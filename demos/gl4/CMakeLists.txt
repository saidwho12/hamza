cmake_minimum_required(VERSION 3.10)
project(hz_gl4_demo)

add_executable(hz_gl4_demo main.c glad/src/glad.c)

if ((${CMAKE_SYSTEM_NAME} MATCHES "Windows") OR WIN32)
    set(PLATFORM_WINDOWS TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(PLATFORM_UNIX TRUE)
endif ()

if (PLATFORM_WINDOWS)
    set(GLFW_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/deps/glfw-3.3.4.bin.WIN64")

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
