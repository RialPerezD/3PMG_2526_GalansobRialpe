conan = {}

configs = {'Debug', 'Release', 'RelWithDebInfo'}

for i = 1,3 do
	include("build/deps/"..configs[i].."/conanbuildinfo.premake.lua")
	conan[configs[i]] = {}
	local cfg = conan[configs[i]]
	cfg["build_type"] = conan_build_type
	cfg["arch"] = conan_arch
	cfg["includedirs"] = conan_includedirs
	cfg["libdirs"] = conan_libdirs
	cfg["bindirs"] = conan_bindirs
	cfg["libs"] = conan_libs
	cfg["system_libs"] = conan_system_libs
	cfg["defines"] = conan_defines
	cfg["cxxflags"] = conan_cxxflags
	cfg["cflags"] = conan_cflags
	cfg["sharedlinkflags"] = conan_sharedlinkflags
	cfg["exelinkflags"] = conan_exelinkflags
	cfg["frameworks"] = conan_frameworks
end


function conan_config_exec()
	configs = {'Debug', 'Release', 'RelWithDebInfo'}
	for i = 1,3 do
		local cfg = conan[configs[i]]
		filter("configurations:"..configs[i])
			linkoptions { cfg["exelinkflags"] }
			includedirs{ cfg["includedirs"] }
			libdirs{ cfg["libdirs"] }
			links{ cfg["libs"] }
			links{ cfg["system_libs"] }
			links{ cfg["frameworks"] }
			defines{ cfg["defines"] }
		filter{}
	end
end

function conan_config_lib()
	configs = {'Debug', 'Release', 'RelWithDebInfo'}
	for i = 1,3 do
		local cfg = conan[configs[i]]
		filter("configurations:"..configs[i])
			linkoptions { cfg["sharedlinkflags"] }
			includedirs{ cfg["includedirs"] }
			defines{ cfg["defines"] }
		filter{}
	end
end


workspace "MotArda"
	configurations {"Debug", "Release", "RelWithDebInfo"}
	architecture "x64"
	location "build"
	cppdialect "c++20"
	startproject "Window"

	includedirs {"include", "deps/**/include/"}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"
		runtime "Debug"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
		runtime "Release"

	filter "configurations:RelWithDebInfo"
		defines { "NDEBUG" }
		optimize "On"
		runtime "Release"
		symbols "On"

	filter {}


	project "MotArda"
		kind "StaticLib"
		targetdir "build/%{cfg.buildcfg}"
		conan_config_lib()
		--pchheader "stdafx.hpp"
		--pchsource "src/stdafx.cpp"
		--forceincludes { "stdafx.hpp" }

		files{
			"premake5.lua",
			"src/build/conanfile.txt",
			--"src/stdafx.cpp", "src/stdafx.hpp",
			"src/Window.cpp", "include/MotArda/Window.hpp",
			"src/Engine.cpp", "include/MotArda/Engine.hpp",
			"src/Input.cpp", "include/MotArda/Input.hpp",
			"src/ObjLoader.cpp", "include/MotArda/ObjLoader.hpp",
			"src/JobSystem.cpp", "include/MotArda/JobSystem.hpp",
			"deps/glad/src/glad.c", "deps/glad/include/glad/glad.h"
			}

	project "WindowExample"
		kind "ConsoleApp"
		language "C++"
		targetdir "build/%{prj.name}/%{cfg.buildcfg}"
		includedirs "include"
		links "MotArda"
		conan_config_exec("Debug")
		conan_config_exec("Release")
		conan_config_exec("RelWithDebInfo")
		debugargs { _MAIN_SCRIPT_DIR .. "/examples/data" }
		files "examples/window.cpp"

	project "TriangleExample"
		kind "ConsoleApp"
		language "C++"
		targetdir "build/%{prj.name}/%{cfg.buildcfg}"
		includedirs "include"
		links "MotArda"
		conan_config_exec("Debug")
		conan_config_exec("Release")
		conan_config_exec("RelWithDebInfo")
		debugargs { _MAIN_SCRIPT_DIR .. "/examples/data" }
		files "examples/triangle.cpp"

	project "MovingTriangleExample"
		kind "ConsoleApp"
		language "C++"
		targetdir "build/%{prj.name}/%{cfg.buildcfg}"
		includedirs "include"
		links "MotArda"
		conan_config_exec("Debug")
		conan_config_exec("Release")
		conan_config_exec("RelWithDebInfo")
		debugargs { _MAIN_SCRIPT_DIR .. "/examples/data" }
		files "examples/movTriangle.cpp"

	project "ObjLoader"
		kind "ConsoleApp"
		language "C++"
		targetdir "build/%{prj.name}/%{cfg.buildcfg}"
		includedirs "include"
		links "MotArda"
		conan_config_exec("Debug")
		conan_config_exec("Release")
		conan_config_exec("RelWithDebInfo")
		debugargs { _MAIN_SCRIPT_DIR .. "/examples/data" }
		files "examples/loadObj.cpp"

