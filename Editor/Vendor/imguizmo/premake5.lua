project "ImGuizmo"
	kind "StaticLib"
	language "C++"

	targetdir ("%{wks.location}/Binaries/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/%{prj.name}")

	files { "**.h", "**.cpp" }

	includedirs {
		"%{wks.location}/Editor/Vendor/imgui"
	}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
