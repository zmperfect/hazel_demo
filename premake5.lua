include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

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