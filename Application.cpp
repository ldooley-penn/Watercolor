#include "Application.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include "Utils/ShaderLoader.h"

#include <glm/gtc/type_ptr.hpp>

#include "OpenGLWrappers/Framebuffer.h"
#include "OpenGLWrappers/FullscreenQuad.h"
#include "OpenGLWrappers/Texture2D.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "Utils/Debug.h"

Application::Application(glm::ivec2 windowSize):
    m_window(nullptr),
    m_defaultProgram(0),
    m_rgbToLuvProgram(0),
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
        const double deltaTime = currentTime - previousTime;
        std::string windowTitle = std::string("Watercolor Rendering. FPS: ")+std::to_string(1.0 / deltaTime);
        glfwSetWindowTitle(m_window, windowTitle.c_str());
        Tick(deltaTime);
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

    m_window = glfwCreateWindow(m_windowSize.x, m_windowSize.y, "Watercolor Rendering. FPS: ", nullptr, nullptr);
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
    m_rgbToLuvProgram = ShaderLoader::createShaderProgram("Shaders/RGBtoLUV.vert", "Shaders/RGBtoLUV.frag");
    m_luvToRgbProgram = ShaderLoader::createShaderProgram("Shaders/LUVtoRGB.vert", "Shaders/LUVtoRGB.frag");
    m_meanShiftProgram = ShaderLoader::createShaderProgram("Shaders/MeanShift.vert", "Shaders/MeanShift.frag");

    m_fullscreenQuad = std::make_unique<FullscreenQuad>();

    std::vector<TextureParameter> textureParameters = {
        {GL_TEXTURE_WRAP_S, GL_REPEAT},
        {GL_TEXTURE_WRAP_T, GL_REPEAT},
        {GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR},
        {GL_TEXTURE_MAG_FILTER, GL_LINEAR}
    };
    m_texture = std::make_unique<Texture2D>("Images/mountains.jpg", textureParameters);

    m_framebufferA = std::make_unique<Framebuffer>(m_windowSize, GL_RGBA32F, GL_RGBA, GL_FLOAT);
    m_framebufferB = std::make_unique<Framebuffer>(m_windowSize, GL_RGBA32F, GL_RGBA, GL_FLOAT);

    // Set up imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init();

    return true;
}

void Application::Tick(double deltaTime)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(200, 100));
    ImGui::Begin("Settings");
    float radius;
    ImGui::InputFloat("Value", &radius, 0.1f, 1.0f, "%.2f");
    ImGui::End();

    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);

    glViewport(0, 0, width, height);

    // Convert to LUV
    glUseProgram(m_rgbToLuvProgram);
    const GLint rgbToLuvTextureUniformLocation = glGetUniformLocation(m_rgbToLuvProgram, "myTexture");
    glUniform1i(rgbToLuvTextureUniformLocation, 0);
    m_texture->Bind(0); // Read Texture
    m_framebufferA->Bind(); // Write Framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_fullscreenQuad->Draw();

    // Run mean shift
    glUseProgram(m_meanShiftProgram);
    const GLint meanShiftTextureUniformLocation = glGetUniformLocation(m_meanShiftProgram, "myTexture");
    glUniform1i(meanShiftTextureUniformLocation, 0);
    if (m_framebufferA->GetColorTexture().expired()) {
        std::cerr << "Failed to get color texture!\n";
        return;
    }
    m_framebufferA->GetColorTexture().lock()->Bind(0); // Read Texture
    const GLint meanShiftTextureSizeUniformLocation = glGetUniformLocation(m_meanShiftProgram, "myTextureSize");
    const glm::ivec2 textureSize = m_framebufferA->GetColorTexture().lock()->GetSize();
    glUniform2f(meanShiftTextureSizeUniformLocation, textureSize.x, textureSize.y);
    m_framebufferB->Bind(); // Write Framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_fullscreenQuad->Draw();

    // Convert to RGB and display
    glUseProgram(m_luvToRgbProgram);
    const GLint luvToRGBTextureUniformLocation = glGetUniformLocation(m_luvToRgbProgram, "myTexture");
    glUniform1i(luvToRGBTextureUniformLocation, 0);
    if (m_framebufferB->GetColorTexture().expired()) {
        std::cerr << "Failed to get color texture!\n";
        return;
    }
    m_framebufferB->GetColorTexture().lock()->Bind(0); // Read Texture
    Framebuffer::Unbind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_fullscreenQuad->Draw();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}