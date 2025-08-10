project "rmlui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

	files
	{
		"Source/**.cpp",
		"Include/RmlUi/Backends/RmlUi_Platform_GLFW.cpp",
		"Include/RmlUi/Backends/RmlUi_Renderer_VK.cpp",
		"Include/RmlUi/Backends/RmlUi_Backend_GLFW_VK.cpp",
		"Include/RmlUi/Backends/RmlUi_Platform_GLFW.h",
		"Include/RmlUi/Backends/RmlUi_Renderer_VK.h",
		"Include/RmlUi/Backends/RmlUi_Include_Vulkan.h",
		"Include/RmlUi/Backends/RmlUi_Vulkan/ShadersCompiledSPV.h"
	}

	removefiles
	{
		"Source/Lottie/**",
		"Source/Lua/**",
		"Source/SVG/**"
	}

	includedirs
	{
		"Include",
		"%{wks.location}/Engine/Vendor/freetype/include",
		"%{wks.location}/Engine/Vendor/glfw/include",
		"%{wks.location}/Engine/Vendor/vulkan-headers/include",
		"%{wks.location}/Engine/Vendor/vma"
	}

	links { "freetype", "glfw" }

	filter "configurations:Debug"
		defines { "RMLUI_DEBUG" }

	filter { }
