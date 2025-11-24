cmake_minimum_required(VERSION 4.0)
project(Watercolor)

set(CMAKE_CXX_STANDARD 20)

file(GLOB IMGUI_SOURCES
        ${PROJECT_SOURCE_DIR}/Libraries/imgui/imgui.cpp
        ${PROJECT_SOURCE_DIR}/Libraries/imgui/imgui_draw.cpp
        ${PROJECT_SOURCE_DIR}/Libraries/imgui/imgui_tables.cpp
        ${PROJECT_SOURCE_DIR}/Libraries/imgui/imgui_widgets.cpp
        ${PROJECT_SOURCE_DIR}/Libraries/imgui/backends/imgui_impl_glfw.cpp
        ${PROJECT_SOURCE_DIR}/Libraries/imgui/backends/imgui_impl_opengl3.cpp
)

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
        OpenGLWrappers/Texture2D.h
        ${IMGUI_SOURCES}
        Camera.cpp
        Camera.h
)

add_subdirectory(${PROJECT_SOURCE_DIR}/Libraries/glfw)
add_subdirectory(${PROJECT_SOURCE_DIR}/Libraries/glad)
add_subdirectory(${PROJECT_SOURCE_DIR}/Libraries/nativefiledialog-extended)
add_subdirectory(${PROJECT_SOURCE_DIR}/Libraries/assimp)

target_link_libraries(${PROJECT_NAME} PRIVATE
        glfw
        glad
        nfd
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/glm
        ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/stb
        ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/assimp/include
)

file(COPY "${CMAKE_SOURCE_DIR}/Shaders/" DESTINATION "${CMAKE_BINARY_DIR}/Shaders/")
file(COPY "${CMAKE_SOURCE_DIR}/Images/" DESTINATION "${CMAKE_BINARY_DIR}/Images/")