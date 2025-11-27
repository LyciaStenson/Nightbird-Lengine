dofile("../../shared_lib_copy.lua")

project "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	
	local outBinDir = "%{wks.location}/Bin/" .. outputdir .. "/%{prj.name}"
	
	targetdir (outBinDir)
	objdir ("%{wks.location}/Int/" .. outputdir .. "/%{prj.name}")

	debugdir (outBinDir)

	defines { "VK_NO_PROTOTYPES" }
	defines { "GLFW_INCLUDE_VULKAN" }
	defines { "IMGUI_IMPL_VULKAN_USE_VOLK" }

	files {
		"Public/**.h",
		"Private/**.cpp"
	}

	includedirs {
		"Public",
		"%{wks.location}/Source/Engine/Public",
		"%{wks.location}/Source/Engine/Modules/Input/Public",
		"%{wks.location}/Source/Vendor/vulkan-headers/include",
		"%{wks.location}/Source/Vendor/volk",
		"%{wks.location}/Source/Vendor/vma",
		"%{wks.location}/Source/Vendor/glfw/include",
		"%{wks.location}/Source/Vendor/glm",
		"%{wks.location}/Source/Vendor/stb",
		"%{wks.location}/Source/Vendor/fastgltf/include",
		"%{wks.location}/Source/Vendor/rttr/src",
		"%{wks.location}/Source/Vendor/json",
		"%{wks.location}/Source/Vendor/imgui",
		"%{wks.location}/Source/Vendor/imguizmo"
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
