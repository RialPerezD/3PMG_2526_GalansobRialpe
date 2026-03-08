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

function physx_config()
    includedirs { "deps/physx/include" }
    
    filter "configurations:Debug"
        libdirs { "deps/physx/lib/debug" }
        links { 
            "PhysX_64", 
            "PhysXCommon_64", 
            "PhysXFoundation_64", 
            "PhysXExtensions_static_64",
            "PhysXPvdSDK_static_64" 
        }

    filter "configurations:Release or RelWithDebInfo"
        libdirs { "deps/physx/lib/release" }
        links { 
            "PhysX_64", 
            "PhysXCommon_64", 
            "PhysXFoundation_64", 
            "PhysXExtensions_static_64",
            "PhysXPvdSDK_static_64"
        }
    filter {}
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

    includedirs {
        "include",
        "deps/**/include",
        "deps/**/src",
        "deps",
        "deps/imgui/"
        }

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
        physx_config()

    files{
        "premake5.lua",
        "src/win64/build/conanfile.txt",
        "src/common/*.cpp", "include/MotArda/common/*.hpp",
        "src/common/Components/*.cpp", "include/MotArda/common/Components/*.hpp",
        "src/common/Systems/*.cpp", "include/MotArda/common/Systems/*.hpp",

        "src/win64/*.cpp", "include/MotArda/win64/*.hpp",
        "src/win64/Systems/*.cpp", "include/MotArda/win64/Systems/*.hpp",
    
        "deps/glad/src/glad.c", "deps/glad/include/glad/glad.h",
        "deps/imgui/*.cpp"
        }

    local example_files = os.matchfiles("examples/*.cpp")

    for _, filepath in ipairs(example_files) do
        local projectName = path.getbasename(filepath)

        project (projectName)
            kind "ConsoleApp"
            language "C++"
            targetdir ("build/" .. projectName .. "/%{cfg.buildcfg}")
            includedirs "include"
            links "MotArda"
            
            conan_config_exec()
            physx_config()
            
            debugargs { _MAIN_SCRIPT_DIR .. "/examples/data" }
            files { filepath }
    end