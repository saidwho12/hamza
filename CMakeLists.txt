cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(Hamza LANGUAGES C CXX)

set(HAMZA_SOURCES
		src/hz-ot.h
		src/hz-ot-shape-complex-arabic.c
		src/hz-ot-shape-complex-arabic.h
		src/hz-ot-shape-complex-arabic-joining-list.h
		src/hz-ot.c
		src/hz-font.h
		src/hz-font.c
		src/hz-face.h
		src/hz-face.c
		src/hz-ft.h
		src/hz.c
		src/hz.h
		src/hz-base.h
		src/util/hz-set.h
		src/util/hz-set.c
		src/util/hz-array.c
		src/util/hz-array.h
		src/util/hz-map.h
		src/util/hz-map.c
		src/util/hz-blob.h
		src/util/hz-blob.c
		src/cmas.h)


#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c89 -pedantic -Wall")

add_library(hamza ${HAMZA_SOURCES})
target_include_directories(hamza PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)

find_package(Freetype REQUIRED)

target_link_libraries(hamza PUBLIC ${FREETYPE_LIBRARIES})
target_include_directories(hamza PUBLIC ${FREETYPE_INCLUDE_DIRS})


add_executable(hamza-example main.c)
target_link_libraries(hamza-example PRIVATE hamza m)