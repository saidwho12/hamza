cmake_minimum_required(VERSION 3.10)
project(hz_benchmark_tests)

add_executable(hz_utf_parser_bench "utf-parser-bench.c")

if ((${CMAKE_SYSTEM_NAME} MATCHES "Windows") OR WIN32)
    set(PLATFORM_WINDOWS TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(PLATFORM_UNIX TRUE)
endif ()

target_include_directories(hz_utf_parser_bench PRIVATE "../")

if (PLATFORM_WINDOWS)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -g -ggdb -fverbose-asm -fno-strict-aliasing")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -ftree-slp-vectorize -fno-strict-aliasing -O2 -Ofast")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --std=gnu99 -march=native")

    if (MINGW)
        set(CMAKE_C_STANDARD_LIBRARIES "${CMAKE_C_STANDARD_LIBRARIES} -static-libgcc -static-libstdc++ -lwsock32 -lws2_32")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
    endif()
elseif (PLATFORM_UNIX)
    # On Linux, link with shared system library.
    target_link_libraries(hz_utf_parser_bench PRIVATE m pthread dl)
endif ()
