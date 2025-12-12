project "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/Binaries/" .. outputdir)
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/Engine")

	defines {
		"VK_NO_PROTOTYPES",
		"VMA_DYNAMIC_VULKAN_FUNCTIONS",
		"GLFW_INCLUDE_NONE",
		"GLFW_INCLUDE_VULKAN"
	}

	filter "system:windows"
		defines { "GLFW_DLL" }
	filter { }

	pic "On"

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

	links { "glfw", "fastgltf", "rttr", "Input" }
