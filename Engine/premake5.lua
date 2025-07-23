project "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

	defines {
		"VK_NO_PROTOTYPES",
		"VMA_DYNAMIC_VULKAN_FUNCTIONS",
		"GLFW_INCLUDE_VULKAN",
		"GLFW_DLL"
	}

	files {
		"Source/**.h",
		"Source/**.cpp"
	}

	includedirs {
		"Source/Public",
		"Modules/Input/Source/Public",
		"Vendor/vulkan-headers/include",
		"Vendor/volk",
		"Vendor/vma",
		"Vendor/glfw/include",
		"Vendor/glm",
		"Vendor/stb",
		"Vendor/fastgltf/include",
		"Vendor/rttr/src",
		"Vendor/json"
	}

	links { "glfw", "fastgltf", "Input" }

	filter { "system:windows" }
		links { "rttr" }
	filter {}
