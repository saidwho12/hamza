cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(Hamza LANGUAGES C)


#ifdef HAMZA_BUILD_SHARED
#option()

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c89 -O3 -Ofast -pedantic -Wall")

set(HAMZA_SOURCES hz/hz.h hz/hz.c)

add_library(hamza ${HAMZA_SOURCES})
target_include_directories(hamza PUBLIC ./)

find_package(Freetype 2.6 REQUIRED)

target_link_libraries(hamza PUBLIC ${FREETYPE_LIBRARIES} m)
target_include_directories(hamza PUBLIC ${FREETYPE_INCLUDE_DIRS})