cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(Hamza VERSION 0.0.1 LANGUAGES C)

set(HAMZA_VERSION_MAJOR 1)
set(HAMZA_VERSION_MINOR 0)
set(HAMZA_VERSION_PATCH 0)
set(HAMZA_VERSION_STRING ${HAMZA_VERSION_MAJOR}.${HAMZA_VERSION_MINOR}.${HAMZA_VERSION_PATCH})

option(HAMZA_BUILD_SHARED "Build Hamza as a shared library" OFF)

if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} --std=gnu17 -m64 -march=native -O0 -g -g3 -ggdb -pedantic")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} --std=gnu17 -m64 -march=native -ftree-slp-vectorize -fprofile-arcs -O3 -Ofast")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --std=gnu17 -m64 -msse4.1 -O0 -g -g3 -ggdb -pedantic")
endif ()

set(HAMZA_SOURCES hz/hz.h hz/hz.c hz/hz_defs.h)

if (HAMZA_BUILD_SHARED)
    add_compile_definitions("HZ_BUILD_SHARED")
    add_library(hamza SHARED ${HAMZA_SOURCES})
else ()
    add_library(hamza STATIC ${HAMZA_SOURCES})
endif ()
target_include_directories(hamza PUBLIC ./)

set_target_properties(hamza PROPERTIES VERSION ${HAMZA_VERSION_STRING}
        SOVERSION ${HAMZA_VERSION_MAJOR})

find_package(Freetype 2.6 REQUIRED)

target_link_libraries(hamza PUBLIC ${FREETYPE_LIBRARIES} m)
target_include_directories(hamza PUBLIC ${FREETYPE_INCLUDE_DIRS})

add_executable(utf_decoder_test ./tests/utf_decoder_test.c)
target_compile_options(utf_decoder_test PRIVATE "-msse4.1" "-O0" "-g")
#target_compile_options(utf_decoder_test PRIVATE "-msse4.1" "-march=native" "-ftree-slp-vectorize" "-fprofile-arcs" "-O3" "-Ofast")
target_link_libraries(utf_decoder_test PRIVATE gcov m)