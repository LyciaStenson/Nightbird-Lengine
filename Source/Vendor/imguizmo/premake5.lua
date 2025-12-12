project "ImGuizmo"
	kind "StaticLib"
	language "C++"

	targetdir ("%{wks.location}/Intermediate/" .. outputdir)
	objdir ("%{wks.location}/Intermediate/" .. outputdir)

	defines {
		"GLFW_INCLUDE_NONE",
		"GLFW_INCLUDE_VULKAN",
		"IMGUI_IMPL_VULKAN_USE_VOLK"
	}

	files { "**.h", "**.cpp" }

	includedirs {
		"%{wks.location}/Source/Vendor/imgui"
	}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
