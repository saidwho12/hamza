cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(Hamza VERSION 0.2.1 LANGUAGES C CXX)

set(HAMZA_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(HAMZA_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(HAMZA_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(HAMZA_VERSION_STRING ${HAMZA_VERSION_MAJOR}.${HAMZA_VERSION_MINOR}.${HAMZA_VERSION_PATCH})

option(HAMZA_BUILD_SHARED "Build Hamza as a shared library" OFF)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} --std=c99 -march=native -O0 -g3 -ggdb -pedantic")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} --std=c99 -march=native -ftree-slp-vectorize -O3 -Ofast")
#    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --std=c99 -march=native -O0 -g -g3 -ggdb -pedantic")
    set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_C_STANDARD_LIBRARIES} -static-libgcc -static-libstdc++ -lwsock32 -lws2_32")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /arch:AVX2")
endif()

set(HAMZA_SOURCES hz/hz.h hz/hz.c hz/hz_defs.h
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

file(TO_CMAKE_PATH $ENV{VULKAN_SDK} LUNARG_SDK_ROOT)
message("LunarG (Vulkan) SDK: ${LUNARG_SDK_ROOT}")
set(GLFW_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/deps/glfw-3.3.4.bin.WIN64")

add_executable(shaping_test "hz/backends/hz_vulkan.c" "hz/backends/hz_vulkan.h" "example/shaping_test.c")
target_link_libraries(shaping_test PRIVATE gdi32 imm32 "${LUNARG_SDK_ROOT}/Lib/vulkan-1.lib" "${GLFW_ROOT_DIR}/lib-mingw-w64/libglfw3dll.a" hamza)
target_include_directories(shaping_test PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}"
        "${LUNARG_SDK_ROOT}/Include"
        "${GLFW_ROOT_DIR}/include")

if (MINGW)
    add_custom_command(TARGET shaping_test POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${GLFW_ROOT_DIR}/lib-mingw-w64/glfw3.dll"
            $<TARGET_FILE_DIR:shaping_test>)
endif()