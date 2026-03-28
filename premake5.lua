workspace "Nightbird"
	configurations { "EditorDebug", "EditorRelease", "AppDebug", "AppRelease" }
	platforms { "Desktop", "WiiU", "3DS" }
	defaultplatform "Desktop"

	exceptionhandling "Off"
	rtti "Off"

	defines { "TOML_COMPILER_HAS_EXCEPTIONS=0" }

	filter { "action:vs*" }
		buildoptions { "/EHs-c-" }
	filter { }

	filter { "platforms:Desktop" }
		architecture "x86_64"
	filter { }

	filter { "configurations:EditorDebug or AppDebug" }
		defines { "DEBUG" }
		symbols "On"
		optimize "Off"
		runtime "Debug"
	filter { }

	filter { "configurations:EditorRelease or AppRelease" }
		defines { "NDEBUG" }
		optimize "On"
		runtime "Release"
	filter { }

	filter { "configurations:EditorDebug or AppDebug" }
		targetsuffix "-Debug"
	filter { "configurations:EditorRelease or AppRelease" }
		targetsuffix "-Release"
	filter { }

	outputdir = "%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "Backends/Libraries/GlfwPlatform/Vendor/glfw"
	include "Editor/Vendor/fastgltf"
	include "Editor/Vendor/rttr"
group ""

group "Nightbird"
	include "Engine"
	include "App"
	include "Editor"
group ""

group "Nightbird/Backends"
	include "Backends/GlfwVulkanBackend"
	include "Backends/WiiUBackend"
	include "Backends/3DSBackend"
group ""

group "Nightbird/Backends/Libraries"
	include "Backends/Libraries/GlfwPlatform"
	include "Backends/Libraries/VulkanRenderer"
group ""
