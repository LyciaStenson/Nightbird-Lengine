project "rttr"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/Binaries/" .. outputdir)
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/rttr")

	defines { "rttr_core_EXPORTS" }

	filter "system:windows"
	defines {
		"RTTR_DLL_EXPORTS",
		"RTTR_DLL"
	}
	filter { }

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"src"
	}
