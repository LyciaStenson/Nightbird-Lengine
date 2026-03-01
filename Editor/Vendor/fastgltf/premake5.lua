project "fastgltf"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/Intermediate/" .. outputdir)
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/fastgltf")

	pic "On"

	files
	{
		"include/**.hpp",
		"src/**.cpp"
	}

	includedirs
	{
		"include"
	}
