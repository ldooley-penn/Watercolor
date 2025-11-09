#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <memory>
#include <unordered_set>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

class Framebuffer;
class Texture2D;
class FullscreenQuad;

class Application {
public:
    explicit Application(glm::ivec2 windowSize = glm::ivec2(640, 480));
    ~Application();

    void Run();

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    void UpdateKeyState(int key, int action);

    void UpdateMouseButtonState(int button, int action);

    void UpdateCursorPosition(double xpos, double ypos);

    void UpdateFramebufferSize(int width, int height);

private:
    bool Initialize();
    void Tick(double deltaTime);


    GLFWwindow* m_window;
    GLuint m_defaultProgram;
    GLuint m_vao;
    GLuint m_vbo;

    std::unordered_set<int> m_keysPressed;

    std::unordered_set<int> m_mouseButtonsPressed;

    glm::dvec2 m_mousePosition;

    glm::ivec2 m_windowSize;

    std::unique_ptr<FullscreenQuad> m_fullscreenQuad;

    std::unique_ptr<Texture2D> m_texture;

    std::unique_ptr<Framebuffer> m_framebufferA;
    std::unique_ptr<Framebuffer> m_framebufferB;
};