project "rttr"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "Off"

	targetdir ("%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

	files
	{
		"**.h",
		"**.h++",
		"**.hpp",
		"**.cpp"
	}

	includedirs
	{
		"src"
	}
