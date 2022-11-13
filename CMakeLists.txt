cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(Hamza VERSION 0.2.1 LANGUAGES C CXX)

set(HAMZA_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(HAMZA_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(HAMZA_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(HAMZA_VERSION_STRING ${HAMZA_VERSION_MAJOR}.${HAMZA_VERSION_MINOR}.${HAMZA_VERSION_PATCH})

option(HAMZA_BUILD_SHARED "Build Hamza as a shared library" OFF)

if ((${CMAKE_SYSTEM_NAME} MATCHES "Windows") OR WIN32)
    message(STATUS "Building Hamza for Windows.")
    set(PLATFORM_WINDOWS TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    message(STATUS "Building Hamza for Linux.")
    set(PLATFORM_UNIX TRUE)
endif ()

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -g2 -ggdb -fverbose-asm")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -ftree-slp-vectorize -O2 -Ofast")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --std=gnu99 -march=native")

    if (MINGW)
        set(CMAKE_C_STANDARD_LIBRARIES "${CMAKE_C_STANDARD_LIBRARIES} -static-libgcc -static-libstdc++ -lwsock32 -lws2_32")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
    endif()
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /arch:AVX2")
endif()

set(HAMZA_SOURCES
        hz/hz.h
        hz/hz.c
        hz/hz_data_tables.h
        hz/hz_glyph_cache.h
        hz/hz_glyph_cache.c)

if(HAMZA_BUILD_SHARED)
    add_compile_definitions("HZ_BUILD_SHARED")
    add_library(hamza SHARED ${HAMZA_SOURCES})
else()
    add_library(hamza STATIC ${HAMZA_SOURCES})
endif()

set_target_properties(hamza PROPERTIES VERSION ${HAMZA_VERSION_STRING}
        SOVERSION ${HAMZA_VERSION_MAJOR})

message(DEBUG "LUNARG Vulkan SDK Path: \"$ENV{VULKAN_SDK}\"")  

if ("$ENV{VULKAN_SDK}" STREQUAL "")
    message(FATAL_ERROR "VULKAN_SDK environment variable must not be set to nothing.")
endif()

file(TO_CMAKE_PATH "$ENV{VULKAN_SDK}" LUNARG_SDK_ROOT)

add_executable(hamza_demo
"demo/hz_demo.c")

if (PLATFORM_WINDOWS)
    set(GLFW_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/deps/glfw-3.3.4.bin.WIN64")

    target_link_libraries(hamza_demo PRIVATE gdi32 imm32 "${LUNARG_SDK_ROOT}/Lib/vulkan-1.lib" "${GLFW_ROOT_DIR}/lib-mingw-w64/libglfw3dll.a" hamza)
    target_include_directories(hamza_demo PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}"
            "${LUNARG_SDK_ROOT}/include"
            "${GLFW_ROOT_DIR}/include"
            )

    if (MINGW)
        add_custom_command(TARGET hamza_demo POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${GLFW_ROOT_DIR}/lib-mingw-w64/glfw3.dll"
                $<TARGET_FILE_DIR:hamza_demo>)
    endif()
elseif (PLATFORM_UNIX)
    # On Linux, link with shared system library.
    target_link_libraries(hamza_demo PRIVATE m pthread dl Xi Xxf86vm Xrandr X11 glfw "${LUNARG_SDK_ROOT}/lib/libvulkan.so" hamza)
    target_include_directories(hamza_demo PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}"
            "${LUNARG_SDK_ROOT}/include")
endif ()

# Compile GLSL shaders to SPIR-V
if (PLATFORM_WINDOWS)
    set(GLSL_COMPILER "${LUNARG_SDK_ROOT}/Bin/glslc.exe")
elseif (PLATFORM_UNIX)
    set(GLSL_COMPILER "${LUNARG_SDK_ROOT}/bin/glslc")
endif ()

set(SHADER_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders/")

add_custom_command(
        COMMAND
        ${CMAKE_COMMAND} -E make_directory ${SHADER_BINARY_DIR}
        OUTPUT ${SHADER_BINARY_DIR}
        COMMENT "Creating ${SHADER_BINARY_DIR}"
        )

set(GLSL_SHADERS ${CMAKE_CURRENT_LIST_DIR}/hz/backends/shaders/bezier_to_sdf.vert
        ${CMAKE_CURRENT_LIST_DIR}/hz/backends/shaders/bezier_to_sdf.frag
        ${CMAKE_CURRENT_LIST_DIR}/hz/backends/shaders/bezier_mask.vert
        ${CMAKE_CURRENT_LIST_DIR}/hz/backends/shaders/bezier_mask.frag)

foreach(source IN LISTS GLSL_SHADERS)
    get_filename_component(FILENAME ${source} NAME)
    add_custom_command(
            COMMAND
            ${GLSL_COMPILER} -o ${SHADER_BINARY_DIR}/${FILENAME}.spv ${source}
            OUTPUT ${SHADER_BINARY_DIR}/${FILENAME}.spv
            DEPENDS ${source} ${SHADER_BINARY_DIR}
            COMMENT "Compiling ${FILENAME}"
            )

    list(APPEND SPV_SHADERS ${SHADER_BINARY_DIR}/${FILENAME}.spv)
endforeach()

add_custom_target(spv_shaders ALL DEPENDS ${SPV_SHADERS})
add_dependencies(hamza_demo spv_shaders)