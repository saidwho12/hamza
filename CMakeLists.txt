cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(Hamza VERSION 0.4.0 LANGUAGES C CXX)

set(HAMZA_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(HAMZA_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(HAMZA_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(HAMZA_VERSION_STRING ${HAMZA_VERSION_MAJOR}.${HAMZA_VERSION_MINOR}.${HAMZA_VERSION_PATCH})

option(HAMZA_BUILD_SHARED "Build Hamza as a shared library" OFF)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
add_subdirectory(curl)

add_executable(update_ucd_ftp update_ucd_ftp.c)
target_link_libraries(update_ucd_ftp PRIVATE regex curl)
add_executable(generate_ucd_headers generate_ucd_headers.c)
target_link_libraries(generate_ucd_headers PRIVATE regex)

add_subdirectory(demos/stbtt_rasterize/)
add_subdirectory(demos/gl4/)
add_subdirectory(tests/)
# add_subdirectory(demos/vulkan/)
# add_subdirectory(demos/gles3/)
