#pragma once

#include "Import/SceneImporter.h"

#include "Core/SceneObject.h"
#include "Core/Mesh.h"
#include "Core/Material.h"
#include "Core/Texture.h"

namespace fastgltf
{
	class Asset;
	struct Mesh;
	struct Image;
}

namespace Nightbird::Core
{
	class SpatialObject;
}

namespace Nightbird::Editor
{
	class GltfSceneImporter : public SceneImporter
	{
	public:
		std::string GetName() const override { return "gltf"; }
		bool SupportsExtension(const std::string& extensions) const override;
		std::unique_ptr<Core::SceneObject> Load(const AssetInfo& assetInfo) override;

	private:
		void ProcessNode(const fastgltf::Asset& gltfAsset, size_t nodeIndex, Core::SpatialObject* parent, const std::vector<std::shared_ptr<Core::Material>>& materials);

		std::vector<std::shared_ptr<Core::Texture>> LoadTextures(const fastgltf::Asset& gltfAsset);
		std::vector<std::shared_ptr<Core::Material>> LoadMaterials(const fastgltf::Asset& gltfAsset, const std::vector<std::shared_ptr<Core::Texture>>& textures);
		std::shared_ptr<Core::Mesh> LoadMesh(const fastgltf::Asset& gltfAsset, const fastgltf::Mesh& gltfMesh, const std::vector<std::shared_ptr<Core::Material>>& materials);

		bool DecodeImage(const fastgltf::Asset& gltfAsset, const fastgltf::Image& image, std::vector<uint8_t>& outPixels, int& outWidth, int& outHeight);
	};
}
