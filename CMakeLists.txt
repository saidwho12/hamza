cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(Hamza LANGUAGES C CXX)

set(CMAKE_C_STANDARD 90)

set(HAMZA_SOURCES
		src/hz-ot.h
		src/hz-ot-shape-complex-arabic.c
		src/hz-ot-shape-complex-arabic.h
		src/hz-ot-shape-complex-arabic-joining-list.h
		src/hz-ot.c
		src/hz-ft.h
		src/hz.c
		src/hz.h
		src/hz-base.h
		src/hz-set.h
		src/hz-set.c
		src/hz-array.c
		src/hz-array.h
		src/hz-map.h
		src/hz-map.c
		main.cpp)

add_library(hamza ${HAMZA_SOURCES})
target_include_directories(hamza PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)

find_package(Freetype REQUIRED)

target_link_libraries(hamza PUBLIC ${FREETYPE_LIBRARIES})
target_include_directories(hamza PUBLIC ${FREETYPE_INCLUDE_DIRS})


add_executable(hamza-example main.cpp)
target_link_libraries(hamza-example PRIVATE hamza)