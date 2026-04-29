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
        "include",
        "deps/**/include",
        "deps",
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
        }

    local example_files = os.matchfiles("examplesSwitch/*.cpp")

    for _, filepath in ipairs(example_files) do
        local projectName = path.getbasename(filepath)

        project (projectName)
            kind "ConsoleApp"
            language "C++"
            targetextension ".elf"
            targetdir ("build/" .. projectName .. "/%{cfg.buildcfg}")
            
            includedirs "include"
            
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

            -- Comando para generar el .nro individual para cada proyecto
            postbuildcommands {
                dkp .. "/tools/bin/elf2nro %{cfg.targetdir}/" .. projectName .. ".elf %{cfg.targetdir}/" .. projectName .. ".nro"
            }
    end