cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(Hamim LANGUAGES C CXX)

set(CMAKE_C_STANDARD 90)

set(HAMIM_SOURCES
		${CMAKE_CURRENT_LIST_DIR}/hamim/hm-ot.h
		${CMAKE_CURRENT_LIST_DIR}/hamim/hm-ot-shape-complex-arabic.c
		${CMAKE_CURRENT_LIST_DIR}/hamim/hm-ot-shape-complex-arabic.h
		${CMAKE_CURRENT_LIST_DIR}/hamim/hm-ot-shape-complex-arabic-joining-list.h
		${CMAKE_CURRENT_LIST_DIR}/hamim/hm-ot.c
        ${CMAKE_CURRENT_LIST_DIR}/hamim/hm-ft.h
		${CMAKE_CURRENT_LIST_DIR}/hamim/hm.c
		${CMAKE_CURRENT_LIST_DIR}/hamim/hm.h
		${CMAKE_CURRENT_LIST_DIR}/hamim/hm-base.h
		${CMAKE_CURRENT_LIST_DIR}/hamim/hm-set.h
		${CMAKE_CURRENT_LIST_DIR}/hamim/hm-set.c
		${CMAKE_CURRENT_LIST_DIR}/hamim/hm-array.c
		${CMAKE_CURRENT_LIST_DIR}/hamim/hm-array.h
		main.cpp)

add_library(hamim ${HAMIM_SOURCES})
target_include_directories(hamim PUBLIC ${CMAKE_CURRENT_LIST_DIR})

find_package(Freetype REQUIRED)

target_link_libraries(hamim PUBLIC ${FREETYPE_LIBRARIES})
target_include_directories(hamim PUBLIC ${FREETYPE_INCLUDE_DIRS})


add_executable(hm-example main.cpp)
target_link_libraries(hm-example PRIVATE hamim)