project "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	removeconfigurations { "AppDebug", "AppRelease" }
	removeplatforms { "WiiU", "3DS" }

	local outBinDir = "%{wks.location}/Binaries/" .. outputdir

	targetdir (outBinDir)
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/%{prj.name}")

	debugdir (outBinDir)

	files {
		"Source/Public/**.h",
		"Source/Private/**.h",
		"Source/Private/**.cpp",
		"%{wks.location}/Engine/Source/Private/Reflect/Registration.cpp"
	}

	includedirs {
		"Source/Public",
		"Source/Private",
		"Vendor/tomlplusplus",
		"Vendor/fastgltf/include",
		"Vendor/dr_libs",
		"Vendor/imgui",
		"%{wks.location}/Engine/Source/Public",
		"%{wks.location}/Engine/Vendor/glm",
		"%{wks.location}/Engine/Vendor/stb",
		"%{wks.location}/Engine/Vendor/stduuid"
	}

	defines { "EDITOR_BUILD" }

	filter { "configurations:EditorDebug or EditorRelease" }
		links { "EditorGlfwVulkanBackend", "EditorGlfwPlatform", "EditorVulkanRenderer", "GlfwVulkanBackend", "GlfwPlatform", "VulkanRenderer", "GLFW", "imgui", "fastgltf", "Engine" }
	filter { }
