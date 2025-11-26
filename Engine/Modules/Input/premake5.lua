project "Input"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/Bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/Int/" .. outputdir .. "/%{prj.name}")

	defines { "INPUT_BUILD" }

	files {
		"Source/**.h",
		"Source/**.cpp"
	}

	includedirs {
		"Source/Public",
		"%{wks.location}/Engine/Vendor/glfw/include"
	}

	links { "glfw" }
