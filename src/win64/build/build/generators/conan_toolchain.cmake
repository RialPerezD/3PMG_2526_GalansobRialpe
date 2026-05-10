

# Conan automatically generated toolchain file
# DO NOT EDIT MANUALLY, it will be overwritten

# Avoid including toolchain file several times (bad if appending to variables like
#   CMAKE_CXX_FLAGS. See https://github.com/android/ndk/issues/323
include_guard()

message(STATUS "Using Conan toolchain: ${CMAKE_CURRENT_LIST_FILE}")

if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeToolchain' generator only works with CMake >= 3.15")
endif()






set(CMAKE_GENERATOR_PLATFORM "x64" CACHE STRING "" FORCE)






# Definition of VS runtime, defined from build_type, compiler.runtime, compiler.runtime_type
cmake_policy(GET CMP0091 POLICY_CMP0091)
if(NOT "${POLICY_CMP0091}" STREQUAL NEW)
    message(FATAL_ERROR "The CMake policy CMP0091 must be NEW, but is '${POLICY_CMP0091}'")
endif()
set(CMAKE_MSVC_RUNTIME_LIBRARY "$<$<CONFIG:Release>:MultiThreadedDLL>$<$<CONFIG:RelWithDebInfo>:MultiThreadedDLL>$<$<CONFIG:Debug>:MultiThreadedDebugDLL>")

string(APPEND CONAN_CXX_FLAGS " /MP16")
string(APPEND CONAN_C_FLAGS " /MP16")

# Extra c, cxx, linkflags and defines


if(DEFINED CONAN_CXX_FLAGS)
  string(APPEND CMAKE_CXX_FLAGS_INIT " ${CONAN_CXX_FLAGS}")
endif()
if(DEFINED CONAN_C_FLAGS)
  string(APPEND CMAKE_C_FLAGS_INIT " ${CONAN_C_FLAGS}")
endif()
if(DEFINED CONAN_SHARED_LINKER_FLAGS)
  string(APPEND CMAKE_SHARED_LINKER_FLAGS_INIT " ${CONAN_SHARED_LINKER_FLAGS}")
endif()
if(DEFINED CONAN_EXE_LINKER_FLAGS)
  string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " ${CONAN_EXE_LINKER_FLAGS}")
endif()

get_property( _CMAKE_IN_TRY_COMPILE GLOBAL PROPERTY IN_TRY_COMPILE )
if(_CMAKE_IN_TRY_COMPILE)
    message(STATUS "Running toolchain IN_TRY_COMPILE")
    return()
endif()

set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON)

# Definition of CMAKE_MODULE_PATH
# The root (which is the default builddirs) path of dependencies in the host context
list(PREPEND CMAKE_MODULE_PATH "C:/Users/David/.conan/data/glfw/3.3.8/_/_/package/87788707645a6a7a0bda278a5f55360eeefe8ff3/" "C:/Users/David/.conan/data/sol2/3.3.0/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/" "C:/Users/David/.conan/data/opengl/system/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/" "C:/Users/David/.conan/data/lua/5.4.6/_/_/package/164640aad040835ac89882393a96d89200694f04/")
# the generators folder (where conan generates files, like this toolchain)
list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

# Definition of CMAKE_PREFIX_PATH, CMAKE_XXXXX_PATH
# The Conan local "generators" folder, where this toolchain is saved.
list(PREPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_LIST_DIR} )
list(PREPEND CMAKE_PROGRAM_PATH "C:/Users/David/.conan/data/glfw/3.3.8/_/_/package/87788707645a6a7a0bda278a5f55360eeefe8ff3/bin" "C:/Users/David/.conan/data/spdlog/1.14.1/_/_/package/7a4b7dbecf529c983055e2bf9c7700819db975cc/bin" "C:/Users/David/.conan/data/lua/5.4.6/_/_/package/164640aad040835ac89882393a96d89200694f04/bin" "C:/Users/David/.conan/data/fmt/10.2.1/_/_/package/2c52a23dc25833dd4323e8144302393f0061b96b/bin")
list(PREPEND CMAKE_LIBRARY_PATH "C:/Users/David/.conan/data/glfw/3.3.8/_/_/package/87788707645a6a7a0bda278a5f55360eeefe8ff3/lib" "C:/Users/David/.conan/data/spdlog/1.14.1/_/_/package/7a4b7dbecf529c983055e2bf9c7700819db975cc/lib" "C:/Users/David/.conan/data/lua/5.4.6/_/_/package/164640aad040835ac89882393a96d89200694f04/lib" "C:/Users/David/.conan/data/fmt/10.2.1/_/_/package/2c52a23dc25833dd4323e8144302393f0061b96b/lib")
list(PREPEND CMAKE_INCLUDE_PATH "C:/Users/David/.conan/data/glfw/3.3.8/_/_/package/87788707645a6a7a0bda278a5f55360eeefe8ff3/include" "C:/Users/David/.conan/data/sol2/3.3.0/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/include" "C:/Users/David/.conan/data/spdlog/1.14.1/_/_/package/7a4b7dbecf529c983055e2bf9c7700819db975cc/include" "C:/Users/David/.conan/data/lua/5.4.6/_/_/package/164640aad040835ac89882393a96d89200694f04/include" "C:/Users/David/.conan/data/fmt/10.2.1/_/_/package/2c52a23dc25833dd4323e8144302393f0061b96b/include")



if (DEFINED ENV{PKG_CONFIG_PATH})
set(ENV{PKG_CONFIG_PATH} "D:/Clase/3PMG_2526_GalansobRialpe/src/win64/build/build/generators;$ENV{PKG_CONFIG_PATH}")
else()
set(ENV{PKG_CONFIG_PATH} "D:/Clase/3PMG_2526_GalansobRialpe/src/win64/build/build/generators;")
endif()




# Variables
# Variables  per configuration


# Preprocessor definitions
# Preprocessor definitions per configuration
