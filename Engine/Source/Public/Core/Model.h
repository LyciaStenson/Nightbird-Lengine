#pragma once

#include <unordered_map>
#include <string>

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>

namespace Nightbird
{
	struct Model
	{
		std::string path;
		fastgltf::Asset gltfAsset;
		
		std::unordered_map<size_t, std::shared_ptr<VulkanTexture>> textures;
		std::vector<std::shared_ptr<Mesh>> meshes;
	};
}