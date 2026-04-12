project "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	pic "On"

	targetdir ("%{wks.location}/Binaries/" .. outputdir)
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/%{prj.name}")

	defines { }
	
	files {
		"Source/Public/**.h",
		"Source/Private/**.h",
		"Source/Private/**.cpp",
	}
	excludes {
		"Source/Private/Reflect/Registration.cpp"
	}

	includedirs {
		"Source/Public",
		"Source/Private",
		"Vendor/glm",
		"Vendor/stb",
		"Vendor/stduuid"
	}

	filter { "configurations:EditorDebug or EditorRelease" }
		includedirs { "%{wks.location}/Editor/Vendor/rttr/src" }
		defines { "EDITOR_BUILD", "RTTR_DLL" }
	filter { }
