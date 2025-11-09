cmake_minimum_required(VERSION 4.0)
project(Watercolor)

set(CMAKE_CXX_STANDARD 20)

add_executable(Watercolor
        main.cpp
        Application.cpp
        Application.h
        Utils/ShaderLoader.h
        Utils/Debug.h
        OpenGLWrappers/Framebuffer.cpp
        OpenGLWrappers/Framebuffer.h
        OpenGLWrappers/FullscreenQuad.cpp
        OpenGLWrappers/FullscreenQuad.h
        OpenGLWrappers/Texture2D.cpp
        OpenGLWrappers/Texture2D.h)

add_subdirectory(${PROJECT_SOURCE_DIR}/Libraries/glfw)
add_subdirectory(${PROJECT_SOURCE_DIR}/Libraries/glad)

file(GLOB IMGUI_SOURCES ${PROJECT_SOURCE_DIR}/Libraries/imgui/*.cpp)
add_library(imgui STATIC ${IMGUI_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
        glfw
        glad
        imgui
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/glm
        ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/stb
        ${PROJECT_SOURCE_DIR}Libraries/imgui
)

file(COPY "${CMAKE_SOURCE_DIR}/Shaders/" DESTINATION "${CMAKE_BINARY_DIR}/Shaders/")
file(COPY "${CMAKE_SOURCE_DIR}/Images/" DESTINATION "${CMAKE_BINARY_DIR}/Images/")