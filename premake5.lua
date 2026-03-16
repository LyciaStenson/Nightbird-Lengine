workspace "Nightbird"
	configurations { "Debug", "Release" }
	platforms { "Desktop", "WiiU", "3DS" }
	exceptionhandling "Off"
	rtti "Off"

	defines { "TOML_COMPILER_HAS_EXCEPTIONS=0" }

	filter { "action:vs*" }
		buildoptions { "/EHs-c-" }
	filter { }

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
	include "Backends/Libraries/GlfwPlatform/Vendor/glfw"
	include "Editor/Vendor/fastgltf"
group ""

group "Nightbird"
	include "Engine"
	include "App"
	include "Editor"
group ""

group "Nightbird/Backends"
	include "Backends/GlfwVulkanBackend"
	include "Backends/WiiUBackend"
group ""

group "Nightbird/Backends/Libraries"
	include "Backends/Libraries/GlfwPlatform"
	include "Backends/Libraries/VulkanRenderer"
group ""
