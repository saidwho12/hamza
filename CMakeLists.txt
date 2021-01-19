cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(guru LANGUAGES C CXX)

set(CMAKE_C_STANDARD 90)

set(GURU_SOURCES
		${CMAKE_CURRENT_LIST_DIR}/guru/guru-ot.h
		${CMAKE_CURRENT_LIST_DIR}/guru/guru-ot-complex-arabic.c
		${CMAKE_CURRENT_LIST_DIR}/guru/guru-ot.c
		${CMAKE_CURRENT_LIST_DIR}/guru/guru-freetype.h
		${CMAKE_CURRENT_LIST_DIR}/guru/guru.c
		${CMAKE_CURRENT_LIST_DIR}/guru/guru.h
		${CMAKE_CURRENT_LIST_DIR}/guru/guru-base.h
		${CMAKE_CURRENT_LIST_DIR}/guru/guru-set.h
		${CMAKE_CURRENT_LIST_DIR}/guru/guru-set.c main.cpp)

add_library(guru ${GURU_SOURCES})
target_include_directories(guru PUBLIC ${CMAKE_CURRENT_LIST_DIR})

find_package(Freetype REQUIRED)

target_link_libraries(guru PUBLIC ${FREETYPE_LIBRARIES})
target_include_directories(guru PUBLIC ${FREETYPE_INCLUDE_DIRS})


add_executable(guru-example main.cpp)
target_link_libraries(guru-example PRIVATE guru)