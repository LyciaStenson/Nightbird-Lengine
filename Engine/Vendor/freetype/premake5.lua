project "freetype"
	kind "StaticLib"
	language "C"

	targetdir ("%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

	defines { "FT2_BUILD_LIBRARY" }

	files
	{
		"**.h",
		"**.c"
	}

	includedirs
	{
		"include",
		"include/freetype",
		"include/freetype/config"
	}
