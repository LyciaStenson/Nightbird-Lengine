project "rmlui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

	defines { "RMLUI_STATIC_LIB" }

	files
	{
		"Source/**.cpp"
	}

	removefiles
	{
		"Source/Lottie/**",
		"Source/Lua/**",
		"Source/SVG/**"
	}

	includedirs
	{
		"Include",
		"%{wks.location}/Engine/Vendor/freetype/include"
	}

	links { "freetype" }

	filter "configurations:Debug"
		defines { "RMLUI_DEBUG" }

	filter { }
