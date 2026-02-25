project "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/Binaries/" .. outputdir)
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/%{prj.name}")

	defines { }
	
	files {
		"Source/Public/**.h",
		"Source/Private/Core/**.h",
		"Source/Private/Core/**.cpp"
	}

	includedirs {
		"Source/Public",
		"Source/Private"
	}

	filter { "platforms:Desktop" }
		files {
			"Source/Private/GLFW/**.h",
			"Source/Private/GLFW/**.cpp",
			"Source/Private/Vulkan/**.h",
			"Source/Private/Vulkan/*.cpp"
		}
		includedirs { "Vendor/glfw/include" }
		links { "GLFW" }
	
	filter { "platforms:WiiU" }
		files {
			"Source/Private/WiiU/**.h",
			"Source/Private/WiiU/**.cpp"
		}
		
	filter { }
