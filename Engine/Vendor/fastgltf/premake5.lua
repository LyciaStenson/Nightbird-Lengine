project "fastgltf"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/Bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/Int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"**.h",
		"**.hpp",
		"**.cpp"
	}

	includedirs
	{
		"include"
	}
