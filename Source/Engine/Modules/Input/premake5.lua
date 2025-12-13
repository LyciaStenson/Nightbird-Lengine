project "Input"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/Binaries/" .. outputdir)
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/Input")

	defines {
		"INPUT_BUILD",
		"GLFW_INCLUDE_NONE"
	}

	files {
		"Public/**.h",
		"Private/**.cpp"
	}

	includedirs {
		"Public",
		"%{wks.location}/Source/Vendor/glfw/include"
	}

	links { "glfw" }
