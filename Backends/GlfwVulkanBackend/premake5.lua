project "GlfwVulkanBackend"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/Binaries/" .. outputdir)
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/%{prj.name}")
	
	defines {
		"VK_NO_PROTOTYPES",
		"VMA_DYNAMIC_VULKAN_FUNCTIONS",
		"GLFW_INCLUDE_VULKAN"
	}
	
	files {
		"Source/Public/**.h",
		"Source/Private/**.h",
		"Source/Private/**.cpp"
	}

	includedirs {
		"Source/Public",
		"Source/Private",
		"%{wks.location}/Engine/Source/Public",
		"%{wks.location}/Engine/Vendor/glm",
		"%{wks.location}/Backends/Libraries/GlfwPlatform/Source/Public",
		"%{wks.location}/Backends/Libraries/GlfwPlatform/Vendor/glfw/include",
		"%{wks.location}/Backends/Libraries/VulkanRenderer/Source/Public",
		"%{wks.location}/Backends/Libraries/VulkanRenderer/Vendor/vulkan-headers/include",
		"%{wks.location}/Backends/Libraries/VulkanRenderer/Vendor/volk",
		"%{wks.location}/Backends/Libraries/VulkanRenderer/Vendor/vma"
	}
	
	links { "GlfwPlatform", "VulkanRenderer" }
