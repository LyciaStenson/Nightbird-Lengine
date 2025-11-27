project "ImGui"
	kind "StaticLib"
	language "C++"

	targetdir ("%{wks.location}/Bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/Int/" .. outputdir .. "/%{prj.name}")

	defines { "IMGUI_IMPL_VULKAN_USE_VOLK" }

	files { "**.h", "**.cpp" }

	includedirs
	{
		"%{wks.location}/Source/Vendor/vulkan-headers/include",
		"%{wks.location}/Source/Vendor/volk",
		"%{wks.location}/Source/Vendor/glfw/include"
	}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
