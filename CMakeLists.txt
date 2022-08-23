cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(Hamza VERSION 0.2.1 LANGUAGES C CXX)

set(HAMZA_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(HAMZA_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(HAMZA_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(HAMZA_VERSION_STRING ${HAMZA_VERSION_MAJOR}.${HAMZA_VERSION_MINOR}.${HAMZA_VERSION_PATCH})

option(HAMZA_BUILD_SHARED "Build Hamza as a shared library" OFF)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} --std=c99 -march=native -O0 -g2 -ggdb -pedantic")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} --std=c99 -march=native -ftree-slp-vectorize -O2 -Ofast")
#    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --std=c99 -march=native -O0 -g -g3 -ggdb -pedantic")
    set(CMAKE_C_STANDARD_LIBRARIES "${CMAKE_C_STANDARD_LIBRARIES} -static-libgcc -static-libstdc++ -lwsock32 -lws2_32")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /arch:AVX2")
endif()

set(HAMZA_SOURCES
        hz/hz.h
        hz/hz.c
        hz/hz-data-tables.h
        hz/hz-glyph-cache.h
        hz/hz-glyph-cache.c)

if(HAMZA_BUILD_SHARED)
    add_compile_definitions("HZ_BUILD_SHARED")
    add_library(hamza SHARED ${HAMZA_SOURCES})
else()
    add_library(hamza STATIC ${HAMZA_SOURCES})
endif()

set_target_properties(hamza PROPERTIES VERSION ${HAMZA_VERSION_STRING}
        SOVERSION ${HAMZA_VERSION_MAJOR})

file(TO_CMAKE_PATH $ENV{VULKAN_SDK} LUNARG_SDK_ROOT)
message("LunarG (Vulkan) SDK: ${LUNARG_SDK_ROOT}")
set(GLFW_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/deps/glfw-3.3.4.bin.WIN64")

add_executable(hamza_demo "hz/backends/hz-vulkan.c" "hz/backends/hz-vulkan.h" "example/shaping-test.c" example/stb_image_write.h)
target_link_libraries(hamza_demo PRIVATE gdi32 imm32 "${LUNARG_SDK_ROOT}/Lib/vulkan-1.lib" "${GLFW_ROOT_DIR}/lib-mingw-w64/libglfw3dll.a" hamza)
target_include_directories(hamza_demo PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}"
        "${LUNARG_SDK_ROOT}/Include"
        "${GLFW_ROOT_DIR}/include")

if (MINGW)
    add_custom_command(TARGET hamza_demo POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${GLFW_ROOT_DIR}/lib-mingw-w64/glfw3.dll"
            $<TARGET_FILE_DIR:hamza_demo>)
endif()

# Compile GLSL shaders to SPIR-V
set(GLSL_COMPILER "${LUNARG_SDK_ROOT}/Bin/glslc.exe")

set(SHADER_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders/")

add_custom_command(
        COMMAND
        ${CMAKE_COMMAND} -E make_directory ${SHADER_BINARY_DIR}
        OUTPUT ${SHADER_BINARY_DIR}
        COMMENT "Creating ${SHADER_BINARY_DIR}"
        )

set(GLSL_SHADERS ${CMAKE_CURRENT_LIST_DIR}/hz/backends/shaders/render-glyph-sdf.vert
        ${CMAKE_CURRENT_LIST_DIR}/hz/backends/shaders/render-glyph-sdf.frag
        ${CMAKE_CURRENT_LIST_DIR}/hz/backends/shaders/bezier-to-sdf.comp)

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