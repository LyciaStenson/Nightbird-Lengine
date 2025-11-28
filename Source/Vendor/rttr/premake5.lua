project "rttr"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/Binaries/" .. outputdir)
	objdir ("%{wks.location}/Intermediate/" .. outputdir)

	defines { "RTTR_DLL_EXPORTS", "RTTR_DLL", "rttr_core_EXPORTS" }

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"src"
	}
