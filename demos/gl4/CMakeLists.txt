cmake_minimum_required(VERSION 3.10)
project(hz_gl4_demo)

add_executable(hz_gl4_demo main.c glad/src/glad.c)

if ((${CMAKE_SYSTEM_NAME} MATCHES "Windows") OR WIN32)
    set(PLATFORM_WINDOWS TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(PLATFORM_UNIX TRUE)
endif ()

if (PLATFORM_WINDOWS)
    set(GLFW_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/deps/glfw-3.3.8.bin.WIN64")

    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g2 -ggdb -fverbose-asm -fno-strict-aliasing")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -ftree-slp-vectorize -fno-strict-aliasing -O2 -Ofast")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --std=gnu99 -march=native")

    if (MINGW)
        set(CMAKE_C_STANDARD_LIBRARIES "${CMAKE_C_STANDARD_LIBRARIES} -static-libgcc -static-libstdc++ -lwsock32 -lws2_32")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
    endif()

    target_link_libraries(hz_gl4_demo PRIVATE gdi32 imm32 OpenGL32 "${GLFW_ROOT_DIR}/lib-mingw-w64/libglfw3dll.a")
    target_include_directories(hz_gl4_demo PRIVATE "${GLFW_ROOT_DIR}/include" "glad/include")

    if (MINGW)
        add_custom_command(TARGET hz_gl4_demo POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${GLFW_ROOT_DIR}/lib-mingw-w64/glfw3.dll"
                $<TARGET_FILE_DIR:hz_gl4_demo>)
    endif()
elseif (PLATFORM_UNIX)
    # On Linux, link with shared system library.
    target_link_libraries(hz_gl4_demo PRIVATE m pthread dl Xi Xxf86vm Xrandr X11 glfw gl)
endif ()
