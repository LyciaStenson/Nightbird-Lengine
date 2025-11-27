project "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/Bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/Int/" .. outputdir .. "/%{prj.name}")

	defines {
		"VK_NO_PROTOTYPES",
		"VMA_DYNAMIC_VULKAN_FUNCTIONS",
		"GLFW_INCLUDE_VULKAN",
		"GLFW_DLL"
	}

	files {
		"Public/**.h",
		"Private/**.cpp"
	}

	includedirs {
		"Public",
		"Modules/Input/Public",
		"%{wks.location}/Source/Vendor/vulkan-headers/include",
		"%{wks.location}/Source/Vendor/volk",
		"%{wks.location}/Source/Vendor/vma",
		"%{wks.location}/Source/Vendor/glfw/include",
		"%{wks.location}/Source/Vendor/glm",
		"%{wks.location}/Source/Vendor/stb",
		"%{wks.location}/Source/Vendor/fastgltf/include",
		"%{wks.location}/Source/Vendor/rttr/src",
		"%{wks.location}/Source/Vendor/json"
	}

	links { "glfw", "fastgltf", "Input" }

	filter { "system:windows" }
		links { "rttr" }
	filter {}
