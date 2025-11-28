project "Input"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/Binaries/" .. outputdir)
	objdir ("%{wks.location}/Intermediate/" .. outputdir)

	defines { "INPUT_BUILD" }

	files {
		"Public/**.h",
		"Private/**.cpp"
	}

	includedirs {
		"Public",
		"%{wks.location}/Source/Vendor/glfw/include"
	}

	links { "glfw" }
