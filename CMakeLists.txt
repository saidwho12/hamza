cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(Hamza VERSION 0.0.1 LANGUAGES C)

set(HAMZA_VERSION_MAJOR 1)
set(HAMZA_VERSION_MINOR 0)
set(HAMZA_VERSION_PATCH 0)
set(HAMZA_VERSION_STRING ${HAMZA_VERSION_MAJOR}.${HAMZA_VERSION_MINOR}.${HAMZA_VERSION_PATCH})

option(HAMZA_BUILD_SHARED "Build Hamza as a shared library" OFF)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} --std=gnu17 -m64 -march=native -O0 -g -g3 -ggdb -pedantic")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} --std=gnu17 -m64 -march=native -ftree-slp-vectorize -fprofile-arcs -O3 -Ofast")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --std=gnu17 -m64 -msse4.1 -O0 -g -g3 -ggdb -pedantic")
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /arch:AVX2")
endif()

set(HAMZA_SOURCES hz/hz.h hz/hz.c hz/hz_defs.h)

if(HAMZA_BUILD_SHARED)
    add_compile_definitions("HZ_BUILD_SHARED")
    add_library(hamza SHARED ${HAMZA_SOURCES})
else()
    add_library(hamza STATIC ${HAMZA_SOURCES})
endif()

set_target_properties(hamza PROPERTIES VERSION ${HAMZA_VERSION_STRING}
        SOVERSION ${HAMZA_VERSION_MAJOR})

# If on windows, use prebuilt VS Express 2017 binaries for FreeType,
# taken from https://github.com/ubawurinna/freetype-windows-binaries.
if(MSVC AND (MSVC_VERSION GREATER_EQUAL 1900))
    target_include_directories(hamza PUBLIC ${CMAKE_CURRENT_LIST_DIR}
            "${CMAKE_CURRENT_LIST_DIR}/FreeType 2.11.0/include")

    if(("${CMAKE_GENERATOR_PLATFORM}" STREQUAL "Win64") OR ("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8"))
        message(STATUS "Linking with Win64 FreeType")
        target_link_libraries(hamza PUBLIC "${CMAKE_CURRENT_LIST_DIR}/FreeType 2.11.0/release static/vs2015-2019/win64/freetype.lib" msvcrt ucrt)
    else()
        message(STATUS "Linking with Win32 FreeType")
        target_link_libraries(hamza PUBLIC "${CMAKE_CURRENT_LIST_DIR}/FreeType 2.11.0/release static/vs2015-2019/win32/freetype.lib" msvcrt ucrt)
    endif()
endif()

if(UNIX)
    target_include_directories(hamza PUBLIC ${CMAKE_CURRENT_LIST_DIR})
    find_package(Freetype 2.6 REQUIRED)
    if(FREETYPE_FOUND)
        set(HAMZA_INCLUDE_DIRS "${HAMZA_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS}")
        target_link_libraries(hamza PUBLIC ${FREETYPE_LIBRARIES} m)
    endif()
endif()

set_target_properties(hamza PROPERTIES VERSION ${HAMZA_VERSION_STRING}
        SOVERSION ${HAMZA_VERSION_MAJOR})
