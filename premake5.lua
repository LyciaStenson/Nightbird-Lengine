workspace "Nightbird"
	configurations { "Debug", "Release" }
	platforms { "Desktop", "WiiU", "3DS" }

	filter { "platforms:Desktop" }
		architecture "x86_64"
	
	filter { }

	startproject "Editor"
	defaultplatform "Desktop"

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

	outputdir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

group "Dependencies"
	include "Engine/Vendor/GLFW"
group ""

group "Nightbird"
	include "Engine"
	include "App"
	include "Editor"
group ""
