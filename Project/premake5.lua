project "Project"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

	files {
		"Source/**.h",
		"Source/**.cpp"
	}

	includedirs {
		"%{wks.location}/Source/Public",
		"%{wks.location}/Vendor/vulkan-headers/include",
		"%{wks.location}/Vendor/volk",
		"%{wks.location}/Vendor/vma",
		"%{wks.location}/Vendor/glfw/include",
		"%{wks.location}/Vendor/glm",
		"%{wks.location}/Vendor/stb",
		"%{wks.location}/Vendor/fastgltf/include",
		"%{wks.location}/Vendor/cereal"
	}

	links { "Engine" }
