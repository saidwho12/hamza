cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(hamza
        VERSION 0.4.0
        DESCRIPTION "Hamza is a small and portable C library for OpenType font shaping and rendering."
        HOMEPAGE_URL "https://github.com/saidwho12/hamza"
        LANGUAGES C)

set(HAMZA_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(HAMZA_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(HAMZA_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(HAMZA_VERSION_STRING ${HAMZA_VERSION_MAJOR}.${HAMZA_VERSION_MINOR}.${HAMZA_VERSION_PATCH})

set(HAMZA_LIB_SHARED OFF CACHE BOOL "Build Hamza as a shared library")
set(HAMZA_BUILD_UCD_PROGS OFF CACHE BOOL "Build UCD programs (requires curl)")
set(HAMZA_NO_STDLIB OFF CACHE BOOL "Build Hamza without the Standard Library")
set(HAMZA_USE_OPENMP OFF CACHE BOOL "Build Hamza with OpenMP support")

if (NOT CMAKE_C_STANDARD)
    message(WARNING "'CMAKE_C_STANDARD' was not set. This will be automatically set to C17.")
    set(CMAKE_C_STANDARD 17)
elseif(NOT CMAKE_C_STANDARD EQUAL 17)
    message(WARNING "Hamza requires C17. 'CMAKE_C_STANDARD' was set to ${CMAKE_C_STANDARD}. "
            "This will be automatically set to C17.")
    set(CMAKE_C_STANDARD 17)
endif()


if (HAMZA_BUILD_UCD_PROGS)
    #set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
    set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
    add_subdirectory(curl)

    add_executable(update_ucd_ftp EXCLUDE_FROM_ALL update_ucd_ftp.c)
    target_link_libraries(update_ucd_ftp PRIVATE regex curl)
    add_executable(generate_ucd_headers EXCLUDE_FROM_ALL generate_ucd_headers.c)
    target_link_libraries(generate_ucd_headers PRIVATE regex)
endif()

set(HAMZA_SOURCES "${CMAKE_CURRENT_LIST_DIR}/hz/hz.c" "${CMAKE_CURRENT_LIST_DIR}/hz/hz.h")

# Use regular if statement for this, could not use generator expressions for it.
if (HAMZA_LIB_SHARED)
    add_library(hamza SHARED ${HAMZA_SOURCES})
else()
    add_library(hamza STATIC ${HAMZA_SOURCES})
endif()

cmake_policy(SET CMP0099 NEW)
cmake_policy(SET CMP0131 NEW) # Use $<LINK_ONLY:...> with LINK_LIBRARIES

set_target_properties(hamza PROPERTIES
                            SOVERSION ${HAMZA_VERSION_STRING}
                            VERSION ${HAMZA_VERSION_MAJOR}.${HAMZA_VERSION_MINOR}
                            DESCRIPTION "Hamza OpenType Library"
                            LINK_FLAGS $<$<BOOL:MINGW>:"-Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive">
                            LINK_LIBRARIES $<LINK_ONLY:$<$<BOOL:MINGW>:"-static-libgcc -static-libstdc++">>
                            INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/hz"
                      )

target_compile_options(hamza PRIVATE 
            $<IF:$<C_COMPILER_ID:GNU>,
                -std=c17 -Oz
                -march=native
                -Wpedantic
                -fstrict-flex-arrays=3
                -faggressive-loop-optimizations
                -funsafe-math-optimizations
                -funroll-loops
                $<$<BOOL:$CACHE{HAMZA_USE_OPENMP}>:-openmp>
              ,$<$<C_COMPILER_ID:MSVC>:
                /std:c17
                /W4
                /favor:AMD64
                /arch:AVX2
                /fp:fast
                /fpcvt:IA
              >
            >
            )

target_compile_definitions(hamza PRIVATE
              HZ_NO_STDLIB=$<BOOL:$CACHE{HAMZA_NO_STDLIB}>
              HZ_USE_OPENMP=$<BOOL:$CACHE{HAMZA_USE_OPENMP}>)

add_subdirectory(demos/gl3/)
# add_subdirectory(tests/)
# add_subdirectory(demos/vulkan/)
# add_subdirectory(demos/gles2/)
