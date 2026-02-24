project "App"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	local outBinDir = "%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}"

	targetdir (outBinDir)
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

	debugdir (outBinDir)

	defines { }

	files {
		"Source/**.h",
		"Source/**.cpp"
	}

	includedirs {
		"Source/Public"
	}

	links { "Engine" }
