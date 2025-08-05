cmake_minimum_required(VERSION 3.14)

project(Apocalypse)


set(SRCS "src/main.cpp")

# Disable unneeded GLFW features
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)


# Resources path macro to resources file path
#target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH"${CMAKE_CURRENT_SOURCE_DIR}/resources/")
# target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH"./resources/") uncomment to send project to someone else


# Set paths to external libraries
set(BGFX_DIR ${CMAKE_SOURCE_DIR}/extern/bgfx)
set(BX_DIR ${CMAKE_SOURCE_DIR}/extern/bx)
set(BIMG_DIR ${CMAKE_SOURCE_DIR}/extern/bimg)
set(GLFW_DIR ${CMAKE_SOURCE_DIR}/extern/glfw)

## Add GLFW to the build
#add_subdirectory(${GLFW_DIR})

include_directories(/opt/homebrew/include/SDL2)
link_directories(/opt/homebrew/lib)


if(APPLE)
    set_source_files_properties(
            ${BGFX_DIR}/src/renderer_vk.cpp
            ${BGFX_DIR}/src/renderer_mtl.mm
            PROPERTIES COMPILE_FLAGS "-x objective-c++"
    )
endif()


# Tell BGFX if we're doing debug or release. Important!
if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    add_compile_definitions(BX_CONFIG_DEBUG=1)
else ()
    add_compile_definitions(BX_CONFIG_DEBUG=0)
endif ()


# CMake function to avoid repetition
function(MakeBGLib arg)
    file(GLOB_RECURSE libsrcs
            "extern/${arg}/src/*.cpp"
            "extern/${arg}/src/*.h"
            "extern/${arg}/include/**/*.h"
            "extern/${arg}/3rdparty/**/*.cpp"
            "extern/${arg}/3rdparty/astc-encoder/source/*.cpp"
    )

    # Exclude amalgamated builds and crt stub
    list(FILTER libsrcs EXCLUDE REGEX ".*amalgamated\\.cpp$")
    list(FILTER libsrcs EXCLUDE REGEX ".*crtnone\\.cpp$")

    if (${arg} STREQUAL "bgfx")
        list(FILTER libsrcs EXCLUDE REGEX ".*glsl-optimizer.*")
        list(FILTER libsrcs EXCLUDE REGEX ".*glslang.*")
        list(FILTER libsrcs EXCLUDE REGEX ".*/node\\.cpp$")
    endif ()

    # Manually add Metal renderer for macOS
    if(${arg} STREQUAL "bgfx" AND APPLE)
        list(APPEND libsrcs "${BGFX_DIR}/src/renderer_mtl.mm")
    endif()



    add_library(${arg} ${libsrcs})

    # Common includes
    target_include_directories(${arg} PRIVATE
            "extern/${arg}/include"
            "extern/${arg}/3rdparty"
    )

    # Additional includes based on library
    if (${arg} STREQUAL "bx")
        # macOS-specific includes
        if (APPLE)
            target_include_directories(${arg} PRIVATE
                    "extern/bx/include/compat/osx"
            )
        elseif (MSVC)
            target_include_directories(${arg} PRIVATE
                    "extern/bx/include/compat/msvc"
            )
        elseif (UNIX)
            target_include_directories(${arg} PRIVATE
                    "extern/bx/include/compat/linux"
            )
        endif ()

        # Ensure __STDC_FORMAT_MACROS is defined for PRIu64, etc.
        target_compile_definitions(${arg} PRIVATE __STDC_FORMAT_MACROS)
    endif ()



    # Optional: Set C++ standard explicitly (defensive)
    set_target_properties(${arg} PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED YES
    )
endfunction()

MakeBGLib(bgfx)
MakeBGLib(bx)
MakeBGLib(bimg)


file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable(Apocalypse ${MY_SOURCES})

target_include_directories(bgfx PRIVATE "${BX_DIR}/include" "${BIMG_DIR}/include"
        "${BGFX_DIR}/3rdparty/directx-headers/include/directx"
        "${BGFX_DIR}/3rdparty/dxsdk/include"
        "${BGFX_DIR}/3rdparty/khronos"
        "${BGFX_DIR}/3rdparty/spirv-tools"
        "${BGFX_DIR}/3rdparty/spirv-tools/include"
        "${BGFX_DIR}/3rdparty/spirv-tools/include/generated"
        "${BGFX_DIR}/3rdparty/spirv-headers/include"
        "${BGFX_DIR}/3rdparty/spirv-cross"

        #        "${BGFX_DIR}/3rdparty/glslang"
        #        "${BGFX_DIR}/3rdparty/glslang/glslang/Include/"
        #        "${BGFX_DIR}/3rdparty/glsl-optimizer/include"
        #        "${BGFX_DIR}/3rdparty/glsl-optimizer/src"
        #        "${BGFX_DIR}/3rdparty/glsl-optimizer/src/mesa"
        #        "${BGFX_DIR}/3rdparty/glsl-optimizer/src/util"
        #        "${BGFX_DIR}/3rdparty/glsl-optimizer/src/glsl"
)

target_compile_definitions(bgfx PRIVATE __STDC_FORMAT_MACROS)

target_include_directories(bx PRIVATE "${BX_DIR}/include")
target_compile_definitions(bx PRIVATE __STDC_FORMAT_MACROS) # Why aren't these already defined by the project itself?

target_include_directories(bimg PRIVATE "${BX_DIR}/include"
        "${BIMG_DIR}/3rdparty/astc-encoder"
        "${BIMG_DIR}/3rdparty/astc-encoder/include"
        "${BIMG_DIR}/3rdparty/iqa/include"
        "${BIMG_DIR}/3rdparty/tinyexr/deps/miniz"
        "${BIMG_DIR}/3rdparty/nvtt")

target_include_directories(Apocalypse SYSTEM PRIVATE "./src"
        "extern/bgfx/include" "extern/bimg/include" "extern/bx/include"
        "extern/glfw-3.3.8/include")

if (MSVC)
    target_include_directories(Apocalypse SYSTEM PRIVATE "extern/bx/include/compat/msvc")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release")
endif ()

# link to the binaries we just defined/built
target_link_libraries(Apocalypse
        -lSDL2
#        glfw ${GLFW_LIBRARIES}
        bgfx bx bimg
        "-framework Metal"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework QuartzCore"
        "-framework MetalKit")

