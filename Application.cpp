#include "Application.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include "Utils/ShaderLoader.h"

#include <glm/gtc/type_ptr.hpp>

#include "OpenGLWrappers/Framebuffer.h"
#include "OpenGLWrappers/FullscreenQuad.h"
#include "OpenGLWrappers/Texture2D.h"
#include "Utils/Debug.h"

Application::Application(glm::ivec2 windowSize):
    m_window(nullptr),
    m_defaultProgram(0),
    m_vao(0),
    m_vbo(0),
    m_mousePosition(glm::dvec2(0, 0)),
    m_windowSize(windowSize),
    m_fullscreenQuad(nullptr),
    m_texture(nullptr)
{

}

Application::~Application()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Application::Run()
{
    if (!Initialize())
    {
        return;
    }

    double previousTime = glfwGetTime();
    while (!glfwWindowShouldClose(m_window))
    {
        const double currentTime = glfwGetTime();
        Tick(currentTime - previousTime);
        previousTime = currentTime;
    }
}

void Application::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
    application->UpdateKeyState(key, action);
}

void Application::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
    application->UpdateMouseButtonState(button, action);
}

void Application::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
    application->UpdateCursorPosition(xpos, ypos);
}

void Application::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
    application->UpdateFramebufferSize(width, height);
}

void Application::UpdateKeyState(const int key, const int action)
{
    if (action == GLFW_PRESS)
    {
        m_keysPressed.insert(key);
    }
    else if (action == GLFW_RELEASE)
    {
        m_keysPressed.erase(key);
    }
}

void Application::UpdateMouseButtonState(int button, int action)
{
    if (action == GLFW_PRESS)
    {
        m_mouseButtonsPressed.insert(button);
    }
    else if (action == GLFW_RELEASE)
    {
        m_mouseButtonsPressed.erase(button);
    }
}

void Application::UpdateCursorPosition(const double xpos, const double ypos)
{
    if (m_mouseButtonsPressed.count(GLFW_MOUSE_BUTTON_RIGHT) > 0)
    {
        const glm::dvec2 mouseDelta = glm::dvec2(xpos, ypos) - m_mousePosition;
    }
    m_mousePosition = glm::dvec2(xpos, ypos);
}

void Application::UpdateFramebufferSize(const int width, const int height)
{
    m_windowSize = glm::ivec2(width, height);

    glViewport(0, 0, m_windowSize.x, m_windowSize.y);
    m_framebufferA->Resize(m_windowSize);
    m_framebufferB->Resize(m_windowSize);
}

bool Application::Initialize()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW!\n";
        glfwTerminate();
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    m_window = glfwCreateWindow(m_windowSize.x, m_windowSize.y, "Watercolor Rendering", nullptr, nullptr);
    if (!m_window)
    {
        std::cerr << "Failed to open GLFW window!\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);

    glfwSetWindowUserPointer(m_window, this);

    glfwSetKeyCallback(m_window, key_callback);

    glfwSetMouseButtonCallback(m_window, mouse_button_callback);

    glfwSetCursorPosCallback(m_window, cursor_position_callback);

    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

    gladLoadGL();

    glfwSwapInterval(1);

    glClearColor(0.5f, 1.0f, 0.75f, 1.0f);

    glViewport(0, 0, m_windowSize.x, m_windowSize.y);

    m_defaultProgram = ShaderLoader::createShaderProgram("Shaders/Default.vert", "Shaders/Default.frag");

    m_fullscreenQuad = std::make_unique<FullscreenQuad>();

    m_texture = std::make_unique<Texture2D>("Images/mountains.jpg");

    m_framebufferA = std::make_unique<Framebuffer>(m_windowSize);
    m_framebufferB = std::make_unique<Framebuffer>(m_windowSize);

    return true;
}

void Application::Tick(double deltaTime)
{
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);

    glViewport(0, 0, width, height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_defaultProgram);

    GLint textureUniformLocation = glGetUniformLocation(m_defaultProgram, "myTexture");
    glUniform1i(textureUniformLocation, 0);

    m_texture->Bind(0); // Read Texture
    m_framebufferA->Bind(); // Write Framebuffer

    // Ping-pong framebuffers
    for (int i = 0; i<3; i++) {
        m_fullscreenQuad->Draw();
        if (i % 2 == 0) {
            m_framebufferA->BindColorTexture(0); // Read Texture
            m_framebufferB->Bind(); // Write Framebuffer
        }
        else {
            m_framebufferB->BindColorTexture(0); // Read Texture
            m_framebufferA->Bind(); // Write Framebuffer
        }
    }

    // Draw to screen
    Framebuffer::Unbind();
    m_fullscreenQuad->Draw();

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}