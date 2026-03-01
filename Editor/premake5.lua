project "Editor"
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
		"Vendor/tomlplusplus",
		"Vendor/fastgltf/include",
		"%{wks.location}/Engine/Source/Public",
		"%{wks.location}/Engine/Vendor/glm",
		"%{wks.location}/Engine/Vendor/stb"
	}

	defines { "EDITOR" }

	links { "Engine" }

	filter { "platforms:Desktop" }
		links { "GlfwVulkanBackend", "GLFW", "fastgltf" }
	filter { }
