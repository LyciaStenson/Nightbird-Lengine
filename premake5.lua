workspace "Nightbird"
	architecture "x86_64"
	startproject "Editor"

	configurations { "Debug", "Release" }

	filter { "configurations:Debug"}
		defines { "DEBUG" }
		symbols "On"
		optimize "Off"
		runtime "Debug"

	filter { "configurations:Release" }
		defines { "NDEBUG" }
		optimize "On"
		runtime "Release"

	filter { }

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "Source/Vendor/glfw"
	include "Source/Vendor/fastgltf"
	include "Source/Vendor/rttr"
	include "Source/Vendor/imgui"
	include "Source/Vendor/imguizmo"
group ""

group "Nightbird"
	include "Source/Engine"
	include "Source/App"
	include "Source/Editor"
group ""

group "Nightbird/Modules"
	include "Source/Engine/Modules/Input"
group ""
