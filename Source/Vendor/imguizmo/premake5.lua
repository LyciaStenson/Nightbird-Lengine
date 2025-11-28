project "ImGuizmo"
	kind "StaticLib"
	language "C++"

	targetdir ("%{wks.location}/Intermediate/" .. outputdir)
	objdir ("%{wks.location}/Intermediate/" .. outputdir)

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
