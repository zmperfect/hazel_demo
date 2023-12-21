include "./vendor/premake/premake_customization/solution_items.lua"

workspace "Hazel_demo"
	architecture "x86_64"
	startproject "Hazelnut"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	solution_items
	{
		".editorconfig"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
-- 包括相对于根文件夹的目录（解决方案目录）
IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/Hazel_demo/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Hazel_demo/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/Hazel_demo/vendor/imgui"
IncludeDir["glm"] = "%{wks.location}/Hazel_demo/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/Hazel_demo/vendor/stb_image"
IncludeDir["entt"] = "%{wks.location}/Hazel_demo/vendor/entt/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/Hazel_demo/vendor/yaml-cpp/include"

group "Dependencies"
	include "vendor/premake"
	include "Hazel_demo/vendor/GLFW"
	include "Hazel_demo/vendor/Glad"
	include "Hazel_demo/vendor/imgui"
	include "Hazel_demo/vendor/yaml-cpp"

group ""

include "Hazel_demo"
include "Sandbox"
include "Hazelnut"