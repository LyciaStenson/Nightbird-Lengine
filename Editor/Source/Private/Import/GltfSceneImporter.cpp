#include "Import/GltfSceneImporter.h"

#include "Import/AssetInfo.h"

#include "Core/SpatialObject.h"
#include "Core/MeshPrimitive.h"
#include "Core/MeshInstance.h"
#include "Core/Transform.h"
#include "Core/Log.h"

#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include <glm/glm.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <string>

namespace Nightbird::Editor
{
	bool GltfSceneImporter::SupportsExtension(const std::string& extension) const
	{
		return extension == ".glb" || extension == ".gltf";
	}
	
	std::unique_ptr<Core::SceneObject> GltfSceneImporter::Load(const AssetInfo& assetInfo)
	{
		fastgltf::Parser parser;

		auto data = fastgltf::GltfDataBuffer::FromPath(assetInfo.sourcePath);
		if (data.error() != fastgltf::Error::None)
		{
			Core::Log::Error("Failed to load file with fastgltf: " + std::string(fastgltf::getErrorMessage(data.error())));
			return nullptr;
		}

		auto gltfAssetExpected = parser.loadGltfBinary(data.get(), assetInfo.sourcePath.parent_path(), fastgltf::Options::None);
		if (gltfAssetExpected.error() != fastgltf::Error::None)
		{
			Core::Log::Error("Failed to parse file with fastgltf: " + std::string(fastgltf::getErrorMessage(gltfAssetExpected.error())));
			return nullptr;
		}

		const fastgltf::Asset& gltfAsset = gltfAssetExpected.get();

		auto textures = LoadTextures(gltfAsset);
		auto materials = LoadMaterials(gltfAsset, textures);

		auto root = std::make_unique<Core::SpatialObject>("ImportedModel");
		root->SetSourceSceneUUID(assetInfo.uuid);

		if (gltfAsset.defaultScene.has_value())
		{
			const fastgltf::Scene& scene = gltfAsset.scenes[gltfAsset.defaultScene.value()];
			
			for (size_t nodeIndex : scene.nodeIndices)
			{
				ProcessNode(gltfAsset, nodeIndex, root.get(), materials);
			}
		}
		else
		{
			Core::Log::Warning("No default scene found in glTF file");
		}
		
		return root;
	}

	void GltfSceneImporter::ProcessNode(const fastgltf::Asset& gltfAsset, size_t nodeIndex, Core::SpatialObject* parent, const std::vector<std::shared_ptr<Core::Material>>& materials)
	{
		const fastgltf::Node& node = gltfAsset.nodes[nodeIndex];
		
		std::unique_ptr<Core::SpatialObject> spatialNode;

		if (node.meshIndex.has_value())
		{
			auto mesh = LoadMesh(gltfAsset, gltfAsset.meshes[node.meshIndex.value()], materials);
			spatialNode = std::make_unique<Core::MeshInstance>(std::string(node.name), mesh);
		}
		else
		{
			spatialNode = std::make_unique<Core::SpatialObject>(std::string(node.name));
		}
		
		if (auto* trs = std::get_if<fastgltf::TRS>(&node.transform))
		{
			spatialNode->transform.position = glm::vec3(trs->translation.x(), trs->translation.y(), trs->translation.z());
			spatialNode->transform.rotation = glm::quat(trs->rotation.w(), trs->rotation.x(), trs->rotation.y(), trs->rotation.z());
			spatialNode->transform.scale = glm::vec3(trs->scale.x(), trs->scale.y(), trs->scale.z());
		}
		
		Core::SpatialObject* target = spatialNode.get();

		for (size_t childIndex : node.children)
			ProcessNode(gltfAsset, childIndex, target, materials);

		parent->AddChild(std::move(spatialNode));
	}
	
	std::vector<std::shared_ptr<Core::Texture>> GltfSceneImporter::LoadTextures(const fastgltf::Asset& gltfAsset)
	{
		std::unordered_map<size_t, std::tuple<std::vector<uint8_t>, int, int>> decodedImages;
		for (size_t i = 0; i < gltfAsset.images.size(); ++i)
		{
			std::vector<uint8_t> pixels;
			int width, height;
			if (DecodeImage(gltfAsset, gltfAsset.images[i], pixels, width, height))
				decodedImages[i] = {std::move(pixels), width, height};
			else
				Core::Log::Error("Failed to decode image at index " + std::to_string(i));
		}
		
		std::vector<std::shared_ptr<Core::Texture>> textures(gltfAsset.textures.size());
		for (size_t i = 0; i < gltfAsset.textures.size(); ++i)
		{
			const auto& gltfTexture = gltfAsset.textures[i];
			if (!gltfTexture.imageIndex.has_value())
				continue;

			size_t imageIndex = gltfTexture.imageIndex.value();
			auto it = decodedImages.find(imageIndex);
			if (it == decodedImages.end())
				continue;

			auto& [pixels, width, height] = it->second;
			textures[i] = std::make_shared<Core::Texture>(static_cast<uint32_t>(width), static_cast<uint32_t>(height), Core::TextureFormat::RGBA8, pixels);
		}

		return textures;
	}
	
	std::vector<std::shared_ptr<Core::Material>> GltfSceneImporter::LoadMaterials(const fastgltf::Asset& gltfAsset, const std::vector<std::shared_ptr<Core::Texture>>& textures)
	{
		std::vector<std::shared_ptr<Core::Material>> materials(gltfAsset.materials.size());

		for (size_t i = 0; i < gltfAsset.materials.size(); ++i)
		{
			const fastgltf::Material& gltfMaterial = gltfAsset.materials[i];
			auto material = std::make_shared<Core::Material>();

			const auto& baseColorFactor = gltfMaterial.pbrData.baseColorFactor;
			material->baseColorFactor = glm::vec4(baseColorFactor.x(), baseColorFactor.y(), baseColorFactor.z(), baseColorFactor.w());
			material->metallicFactor = gltfMaterial.pbrData.metallicFactor;
			material->roughnessFactor = gltfMaterial.pbrData.roughnessFactor;
			material->transparencyEnabled = (gltfMaterial.alphaMode == fastgltf::AlphaMode::Blend);
			material->doubleSided = gltfMaterial.doubleSided;

			if (gltfMaterial.pbrData.baseColorTexture.has_value())
			{
				size_t textureIndex = gltfMaterial.pbrData.baseColorTexture->textureIndex;
				if (textureIndex < textures.size() && textures[textureIndex])
					material->baseColorTexture = textures[textureIndex];
			}

			if (gltfMaterial.pbrData.metallicRoughnessTexture.has_value())
			{
				size_t textureIndex = gltfMaterial.pbrData.metallicRoughnessTexture->textureIndex;
				if (textureIndex < textures.size() && textures[textureIndex])
					material->metallicRoughnessTexture = textures[textureIndex];
			}

			if (gltfMaterial.normalTexture.has_value())
			{
				size_t textureIndex = gltfMaterial.normalTexture->textureIndex;
				if (textureIndex < textures.size() && textures[textureIndex])
					material->normalTexture = textures[textureIndex];
			}

			materials[i] = std::move(material);
		}

		return materials;
	}
	
	std::shared_ptr<Core::Mesh> GltfSceneImporter::LoadMesh(const fastgltf::Asset& gltfAsset, const fastgltf::Mesh& gltfMesh, const std::vector<std::shared_ptr<Core::Material>>& materials)
	{
		std::vector<Core::MeshPrimitive> primitives;

		for (const fastgltf::Primitive& gltfPimitive : gltfMesh.primitives)
		{
			std::vector<Core::Vertex> vertices;
			std::vector<uint16_t> indices;

			if (auto it = gltfPimitive.findAttribute("POSITION"); it != gltfPimitive.attributes.end())
			{
				const auto& accessor = gltfAsset.accessors[it->accessorIndex];
				vertices.resize(accessor.count);
				fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(gltfAsset, accessor,
					[&](fastgltf::math::fvec3 vec, size_t i)
					{
						vertices[i].position = glm::vec3(vec.x(), vec.y(), vec.z());
					}
				);
			}

			if (auto it = gltfPimitive.findAttribute("NORMAL"); it != gltfPimitive.attributes.end())
			{
				const auto& accessor = gltfAsset.accessors[it->accessorIndex];
				fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(gltfAsset, accessor,
					[&](fastgltf::math::fvec3 vec, size_t i)
					{
						vertices[i].normal = glm::vec3(vec.x(), vec.y(), vec.z());
					}
				);
			}

			if (gltfPimitive.indicesAccessor.has_value())
			{
				const auto& accessor = gltfAsset.accessors[gltfPimitive.indicesAccessor.value()];
				fastgltf::iterateAccessorWithIndex<uint16_t>(gltfAsset, accessor,
					[&](uint16_t index, size_t)
					{
						indices.push_back(index);
					}
				);
			}

			size_t baseColorTexcoordIndex = 0;
			size_t metallicRoughnessTexcoordIndex = 0;
			size_t normalTexCoordIndex = 0;
			std::shared_ptr<Core::Material> material;

			if (gltfPimitive.materialIndex.has_value())
			{
				size_t materialIndex = gltfPimitive.materialIndex.value();
				material = materials[materialIndex];
				const fastgltf::Material& gltfMaterial = gltfAsset.materials[materialIndex];

				if (gltfMaterial.pbrData.baseColorTexture.has_value())
				{
					if (gltfMaterial.pbrData.baseColorTexture->transform && gltfMaterial.pbrData.baseColorTexture->transform->texCoordIndex.has_value())
						baseColorTexcoordIndex = gltfMaterial.pbrData.baseColorTexture->transform->texCoordIndex.value();
					else
						baseColorTexcoordIndex = gltfMaterial.pbrData.baseColorTexture->texCoordIndex;
				}

				if (gltfMaterial.pbrData.metallicRoughnessTexture.has_value())
				{
					if (gltfMaterial.pbrData.metallicRoughnessTexture->transform && gltfMaterial.pbrData.metallicRoughnessTexture->transform->texCoordIndex.has_value())
						metallicRoughnessTexcoordIndex = gltfMaterial.pbrData.metallicRoughnessTexture->transform->texCoordIndex.value();
					else
						metallicRoughnessTexcoordIndex = gltfMaterial.pbrData.metallicRoughnessTexture->texCoordIndex;
				}

				if (gltfMaterial.normalTexture.has_value())
				{
					if (gltfMaterial.normalTexture->transform && gltfMaterial.normalTexture->transform->texCoordIndex.has_value())
						normalTexCoordIndex = gltfMaterial.normalTexture->transform->texCoordIndex.value();
					else
						normalTexCoordIndex = gltfMaterial.normalTexture->texCoordIndex;
				}
			}

			auto loadTexCoord = [&](size_t texCoordIndex, auto setter)
				{
					auto attr = std::string("TEXCOORD_") + std::to_string(texCoordIndex);
					if (auto it = gltfPimitive.findAttribute(attr); it != gltfPimitive.attributes.end())
					{
						const auto& accessor = gltfAsset.accessors[it->accessorIndex];
						if (accessor.bufferViewIndex.has_value())
						{
							fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(gltfAsset, accessor,
								[&](fastgltf::math::fvec2 uv, size_t i)
								{
									setter(vertices[i], glm::vec2(uv.x(), uv.y()));
								});
						}
					}
				};
			
			loadTexCoord(baseColorTexcoordIndex, [](Core::Vertex& vertex, glm::vec2 uv) { vertex.baseColorTexCoord = uv; });
			loadTexCoord(metallicRoughnessTexcoordIndex, [](Core::Vertex& vertex, glm::vec2 uv) { vertex.metallicRoughnessTexCoord = uv; });
			loadTexCoord(normalTexCoordIndex, [](Core::Vertex& vertex, glm::vec2 uv) { vertex.normalTexCoord = uv; });

			if (!material)
				material = std::make_shared<Core::Material>();

			primitives.emplace_back(std::move(vertices), std::move(indices), material);
		}

		return std::make_shared<Core::Mesh>(std::move(primitives));
	}
	
	bool GltfSceneImporter::DecodeImage(const fastgltf::Asset& gltfAsset, const fastgltf::Image& image, std::vector<uint8_t>& outPixels, int& outWidth, int& outHeight)
	{
		bool decoded = false;

		std::visit(fastgltf::visitor
			{
				[&](const fastgltf::sources::BufferView& view)
			{
				const auto& bufferView = gltfAsset.bufferViews[view.bufferViewIndex];
				const auto& buffer = gltfAsset.buffers[bufferView.bufferIndex];

				std::visit(fastgltf::visitor
					{
						[&](const fastgltf::sources::Array& array)
					{
						int channels;
						stbi_set_flip_vertically_on_load(false);
						unsigned char* pixels = stbi_load_from_memory
						(
							reinterpret_cast<const stbi_uc*>(array.bytes.data() + bufferView.byteOffset),
							static_cast<int>(bufferView.byteLength),
							&outWidth, &outHeight, &channels, 4
						);
						if (pixels)
						{
							outPixels.assign(pixels, pixels + (outWidth * outHeight * 4));
							stbi_image_free(pixels);
							decoded = true;
						}
						else
							Core::Log::Error("Failed to decode image with stb_image");
					},
					[](auto&) {}
				}, buffer.data);
			},
			[&](const fastgltf::sources::URI&)
			{
				Core::Log::Warning("Exteneral textures not supported, please use .glb");
			},
			[](auto&) {}
			}, image.data);
		
		return decoded;
	}
}
