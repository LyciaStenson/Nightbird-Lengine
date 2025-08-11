project "rmlui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

	defines {
		"RMLUI_FONT_ENGINE_FREETYPE",
		"VK_NO_PROTOTYPES",
		"VMA_DYNAMIC_VULKAN_FUNCTIONS",
		"GLFW_INCLUDE_VULKAN",
		"GLFW_DLL"
	}

	files
	{
		"Source/**.cpp"
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
		"%{wks.location}/Engine/Vendor/volk",
		"%{wks.location}/Engine/Vendor/vma"
	}

	links { "freetype", "glfw" }

	filter "configurations:Debug"
		defines { "RMLUI_DEBUG" }
	filter { }
