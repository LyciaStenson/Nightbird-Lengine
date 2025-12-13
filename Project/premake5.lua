local enginePath = os.getenv("NIGHTBIRD_PATH")
assert(enginePath, "NIGHTBIRD_PATH environment variable not found.")

enginePath = enginePath:gsub("\\", "/")
if enginePath:sub(-1) ~= "/" then
	enginePath = enginePath .. "/"
end

workspace "@PROJECT_NAME@"
	architecture "x86_64"
	startproject "Editor"

	configurations { "Debug", "Release" }

	filter { "configurations:Debug"}
		defines { "DEBUG" }
		symbols "On"
		optimize "Off"
		runtime "Debug"

	filter { "configurations:Release" }
		defines { "NDEBUG" }
		optimize "On"
		runtime "Release"

	filter { }

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Editor"
	kind "None"

	targetdir ("%{wks.location}/Binaries/" .. outputdir)
	objdir ("%{wks.location}/Intermediate/" .. outputdir)

	debugcommand (enginePath .. "Binaries/" .. outputdir .. "/Editor.exe")
	debugdir (enginePath .. "Binaries/" .. outputdir)

	debugargs { "%{wks.location}/Binaries/" .. outputdir .. "/%{wks.name}" }

	dependson { "@PROJECT_NAME@" }

project "@PROJECT_NAME@"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/Binaries/" .. outputdir)
	objdir ("%{wks.location}/Intermediate/" .. outputdir)

	defines { "PROJECT_BUILD" }

	files {
		"Source/**.h",
		"Source/**.cpp"
	}

	includedirs {
		"Source/Public",
		enginePath .. "Source/Engine/Public",
		enginePath .. "Source/Engine/Modules/Input/Public",
		enginePath .. "Source/Vendor/vulkan-headers/include",
		enginePath .. "Source/Vendor/volk",
		enginePath .. "Source/Vendor/vma",
		enginePath .. "Source/Vendor/glfw/include",
		enginePath .. "Source/Vendor/glm",
		enginePath .. "Source/Vendor/stb",
		enginePath .. "Source/Vendor/fastgltf/include",
		enginePath .. "Source/Vendor/rttr/src",
		enginePath .. "Source/Vendor/json"
	}
	
	libdirs { enginePath .. "Binaries/" .. outputdir }
	
	links { "Engine", "Input", "rttr" }
