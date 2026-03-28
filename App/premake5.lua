project "App"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	removeconfigurations { "EditorDebug", "EditorRelease" }

	local outBinDir = "%{wks.location}/Binaries/" .. outputdir

	targetdir (outBinDir)
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/%{prj.name}")

	debugdir (outBinDir)

	files {
		"Source/Public/**.h",
		"Source/Private/**.h",
		"Source/Private/**.cpp"
	}

	includedirs {
		"Source/Public",
		"Source/Private",
		"%{wks.location}/Engine/Source/Public",
		"%{wks.location}/Engine/Vendor/glm",
		"%{wks.location}/Engine/Vendor/stb",
		"%{wks.location}/Engine/Vendor/stduuid"
	}

	filter { "platforms:Desktop" }
		links { "GlfwVulkanBackend", "GlfwPlatform", "VulkanRenderer", "GLFW", "Engine" }
	filter { }

	filter { "platforms:WiiU" }
		links { "WiiUBackend", "Engine" }
	filter { }
