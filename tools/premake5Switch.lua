local dkp = os.getenv("DEVKITPRO")
local dka = os.getenv("DEVKITARM")

if not dkp then
    error("DEVKITPRO NOT FOUND")
end

workspace "MotArda"
    configurations {"Debug", "Release", "RelWithDebInfo"}
    platforms { "Switch" }
    defaultplatform "Switch"
    
    location "build"
    cppdialect "c++20"
    architecture "ARM" 

    buildoptions {
        "-march=armv8-a+crc+crypto",
        "-mtune=cortex-a57",
        "-mtp=soft",
        "-fPIE"
    }

    includedirs {
        "include/common",
        "include/switch",
        "deps/FastNoise/include",
        "deps/stb_image/include",
        "deps/tiny_obj/include",
        "deps/imgui/",
        dkp .. "/libnx/include",
        dkp .. "/portlibs/switch/include"
    }

    filter "configurations:Debug"
        defines { "DEBUG", "__SWITCH__" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG", "__SWITCH__" }
        optimize "On"

    filter "configurations:RelWithDebInfo"
        defines { "NDEBUG", "__SWITCH__" }
        optimize "On"
        symbols "On"

    filter {}

    project "MotArda"
        kind "StaticLib"
        language "C++"
        targetdir "build/lib/%{cfg.buildcfg}"
        
        files {
        "src/common/*.cpp", "include/common/MotArda/*.hpp",
        "src/common/Components/*.cpp", "include/common/MotArda/Components/*.hpp",
        "src/common/Systems/*.cpp", "include/common/MotArda/Systems/*.hpp",
        "src/common/CardGame/*.cpp", "include/common/MotArda/CardGame/*.hpp",

        "src/switch/*.cpp", "include/switch/MotArda/*.hpp",
        "src/switch/Systems/*.cpp", "include/switch/MotArda/Systems/*.hpp",
    
        "deps/glad/src/glad.c", "deps/glad/include/glad/glad.h",
        "deps/imgui/*.cpp",
        }

    local example_files = os.matchfiles("examplesSwitch/**/*.cpp")

for _, filepath in ipairs(example_files) do
        local directory = path.getdirectory(filepath)
        local projectName = path.getname(directory)

        project (projectName)
            kind "ConsoleApp"
            language "C++"
            targetextension ".elf"
            targetdir ("build/examples/" .. projectName .. "/%{cfg.buildcfg}")
            objdir ("build/obj/" .. projectName .. "/%{cfg.buildcfg}")
            
            includedirs { 
                "include/common", 
                "include/switch",
                dkp .. "/libnx/include",
                dkp .. "/portlibs/switch/include" 
            }
            
            libdirs { 
                dkp .. "/libnx/lib",
                dkp .. "/portlibs/switch/lib"
            }
            
            links { "MotArda", "nx", "m" }

            linkoptions {
                "-specs=" .. dkp .. "/libnx/switch.specs",
                "-g",
                "-march=armv8-a+crc+crypto",
                "-mtune=cortex-a57",
                "-mtp=soft",
                "-fPIE"
            }

            files { filepath }

            postbuildcommands {
                dkp .. "/tools/bin/elf2nro %{cfg.targetdir}/" .. projectName .. ".elf %{cfg.targetdir}/" .. projectName .. ".nro"
            }
    end