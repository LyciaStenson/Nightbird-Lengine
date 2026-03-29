project "rttr"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"

	exceptionhandling "On"
	rtti "On"
	pic "On"

	removeconfigurations { "AppDebug", "AppRelease" }
	removeplatforms { "WiiU", "3DS" }

	targetdir ("%{wks.location}/Binaries/" .. outputdir)
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/rttr")

	defines { "rttr_core_EXPORTS" }

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"src"
	}

	filter { "system:windows" }
		defines { "RTTR_DLL_EXPORTS", "RTTR_DLL" }
	filter { }
