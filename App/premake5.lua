project "App"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	local outBinDir = "%{wks.location}/Binaries/" .. outputdir

	targetdir (outBinDir)
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/%{prj.name}")

	debugdir (outBinDir)

	defines { }

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
	}

	links { "Engine" }

	filter { "platforms:Desktop" }
		links { "GlfwPlatform", "VulkanRenderer", "GLFW" }
	filter { }

	filter { "platforms:WiiU" }
		links { "WiiUBackend" }
	filter { }