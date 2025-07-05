#pragma once

#include <unordered_map>
#include <memory>
#include <string>

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>

#include <Core/MeshPrimitive.h>

namespace Nightbird
{
	class VulkanTexture;
	class Mesh;

	struct TextureData
	{
		std::vector<uint8_t> pixels;
		int width;
		int height;
		int channels;
		bool sRGB;
	};

	struct MeshData
	{
		std::vector<MeshPrimitiveInfo> primitiveInfo;
	};

	struct Model
	{
		std::string path;
		fastgltf::Asset gltfAsset;

		std::vector<TextureData> textureData;
		std::vector<MeshData> meshData;
		
		std::unordered_map<size_t, std::shared_ptr<VulkanTexture>> textures;
		std::vector<std::shared_ptr<Mesh>> meshes;
	};
}