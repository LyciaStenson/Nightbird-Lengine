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
		"Source/Private",
		"Vendor/glm"
	}
