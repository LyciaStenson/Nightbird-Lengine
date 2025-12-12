project "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	
	local outBinDir = "%{wks.location}/Binaries/" .. outputdir

	targetdir (outBinDir)
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/Editor")

	debugdir (outBinDir)

	defines {
		"VK_NO_PROTOTYPES",
		"GLFW_INCLUDE_NONE",
		"GLFW_INCLUDE_VULKAN",
		"IMGUI_IMPL_VULKAN_USE_VOLK"
	}

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

	links { "Engine", "glfw", "fastgltf", "rttr", "imgui", "imguizmo", "Input" }

	filter "system:linux"
		links { "X11", "Xrandr", "Xi", "Xinerama", "Xcursor" }
