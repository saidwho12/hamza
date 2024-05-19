cmake_minimum_required(VERSION 3.10)
project(gl3_demo)

add_executable(gl3_demo hz_gl3.h hz_gl3.c main.c glad/src/glad.c)

set(CMAKE_C_STANDARD 17)

if ((${CMAKE_SYSTEM_NAME} MATCHES "Windows") OR WIN32)
    set(PLATFORM_WINDOWS TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(PLATFORM_UNIX TRUE)
endif ()

if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g2 -ggdb -fverbose-asm -fno-strict-aliasing")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -ftree-slp-vectorize -fno-strict-aliasing -O2 -Ofast")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --std=c17 -march=native")
endif()

if (PLATFORM_WINDOWS)
    set(GLFW_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/deps/glfw-3.3.8.bin.WIN64")

    if (MINGW)
        set(CMAKE_C_STANDARD_LIBRARIES "${CMAKE_C_STANDARD_LIBRARIES} -static-libgcc -static-libstdc++ -lwsock32 -lws2_32")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
    endif()

    target_include_directories(gl3_demo PRIVATE "${GLFW_ROOT_DIR}/include" "glad/include")
    
    if (MINGW)
        target_link_libraries(gl3_demo PRIVATE gdi32 imm32 OpenGL32 hamza "${GLFW_ROOT_DIR}/lib-mingw-w64/libglfw3dll.a")
        add_custom_command(TARGET gl3_demo POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${GLFW_ROOT_DIR}/lib-mingw-w64/glfw3.dll"
                $<TARGET_FILE_DIR:gl3_demo>)
    else()
        target_link_libraries(gl3_demo PRIVATE gdi32 imm32 OpenGL32 hamza "${GLFW_ROOT_DIR}/lib-static-ucrt/glfw3dll.lib")
        add_custom_command(TARGET gl3_demo POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${GLFW_ROOT_DIR}/lib-static-ucrt/glfw3.dll"
                $<TARGET_FILE_DIR:gl3_demo>)
    endif()
elseif (PLATFORM_UNIX)
    # On Linux, link with shared system library.
    target_link_libraries(gl3_demo PRIVATE m pthread dl Xi Xxf86vm Xrandr X11 glfw GL hamza)
    target_include_directories(gl3_demo PRIVATE "${GLFW_ROOT_DIR}/include" "glad/include")
endif ()

add_custom_command(TARGET gl3_demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/data/fonts/TimesNewRoman.ttf"
    $<TARGET_FILE_DIR:gl3_demo>)
add_custom_command(TARGET gl3_demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/data/fonts/Quran/OmarNaskh-Regular.ttf"
    $<TARGET_FILE_DIR:gl3_demo>)
add_custom_command(TARGET gl3_demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/data/fonts/Orbit-Regular.ttf"
    $<TARGET_FILE_DIR:gl3_demo>)