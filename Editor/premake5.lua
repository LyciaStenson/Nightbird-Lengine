dofile("../shared_lib_copy.lua")

project "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	
	local outBinDir = "%{wks.location}/Binaries/" .. outputdir .. "/%{prj.name}"
	
	targetdir (outBinDir)
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/%{prj.name}")

	debugdir (outBinDir)

	defines { "VK_NO_PROTOTYPES" }
	defines { "GLFW_INCLUDE_VULKAN" }
	defines { "IMGUI_IMPL_VULKAN_USE_VOLK" }

	files {
		"Source/**.h",
		"Source/**.cpp"
	}

	includedirs {
		"Source/Public",
		"%{wks.location}/Engine/Source/Public",
		"%{wks.location}/Engine/Modules/Input/Source/Public",
		"%{wks.location}/Engine/Vendor/vulkan-headers/include",
		"%{wks.location}/Engine/Vendor/volk",
		"%{wks.location}/Engine/Vendor/vma",
		"%{wks.location}/Engine/Vendor/glfw/include",
		"%{wks.location}/Engine/Vendor/glm",
		"%{wks.location}/Engine/Vendor/stb",
		"%{wks.location}/Engine/Vendor/fastgltf/include",
		"%{wks.location}/Engine/Vendor/rttr/src",
		"%{wks.location}/Engine/Vendor/json",
		"%{wks.location}/Editor/Vendor/imgui",
		"%{wks.location}/Editor/Vendor/imguizmo"
	}

	defines { "EDITOR" }

	links { "Engine", "imgui", "imguizmo" }

	filter { "system:windows" }
		links { "rttr" }
		copy_shared_lib("rttr", "windows", outputdir)
		copy_shared_lib("glfw", "windows", outputdir)
		copy_shared_lib("Input", "windows", outputdir)
	filter { "system:linux" }
		copy_shared_lib("rttr", "linux", outputdir)
		copy_shared_lib("glfw", "linux", outputdir)
		copy_shared_lib("Input", "linux", outputdir)
	filter {}
