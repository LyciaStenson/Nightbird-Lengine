project "Input"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

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
