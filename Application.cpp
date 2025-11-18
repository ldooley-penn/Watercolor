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

Application::Application(glm::ivec2 windowSize) :
    m_window(nullptr),
    m_defaultProgram(0),
    m_rgbToLuvProgram(0), m_luvToRgbProgram(0), m_meanShiftProgram(0),
    m_gradientProgram(0),
    m_mousePosition(glm::dvec2(0, 0)),
    m_windowSize(windowSize),
    m_fullscreenQuad(nullptr),
    m_texture(nullptr),
    m_paperTexture(nullptr)
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

    m_defaultProgram = ShaderLoader::createShaderProgram("Shaders/Default.vert", "Shaders/Default.frag");
    m_rgbToLuvProgram = ShaderLoader::createShaderProgram("Shaders/RGBtoLUV.vert", "Shaders/RGBtoLUV.frag");
    m_luvToRgbProgram = ShaderLoader::createShaderProgram("Shaders/LUVtoRGB.vert", "Shaders/LUVtoRGB.frag");
    m_meanShiftProgram = ShaderLoader::createShaderProgram("Shaders/MeanShift.vert", "Shaders/MeanShift.frag");
    m_gradientProgram = ShaderLoader::createShaderProgram("Shaders/Gradient.vert", "Shaders/Gradient.frag");
    m_wobbleProgram = ShaderLoader::createShaderProgram("Shaders/Wobble.vert", "Shaders/Wobble.frag");

    m_fullscreenQuad = std::make_unique<FullscreenQuad>();

    std::vector<TextureParameter> textureParametersClampToBorder = {
        {GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER},
        {GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER},
        {GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR},
        {GL_TEXTURE_MAG_FILTER, GL_LINEAR}
    };

    std::vector<TextureParameter> textureParametersRepeat = {
        {GL_TEXTURE_WRAP_S, GL_REPEAT},
        {GL_TEXTURE_WRAP_T, GL_REPEAT},
        {GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR},
        {GL_TEXTURE_MAG_FILTER, GL_LINEAR}
    };

    std::vector<TextureParameter> textureParametersNoMipmapClampToBorder = {
        {GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER},
        {GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER},
        {GL_TEXTURE_MIN_FILTER, GL_LINEAR},
        {GL_TEXTURE_MAG_FILTER, GL_LINEAR}
    };

    std::vector<TextureParameter> textureParametersNoMipmapRepeat = {
        {GL_TEXTURE_WRAP_S, GL_REPEAT},
        {GL_TEXTURE_WRAP_T, GL_REPEAT},
        {GL_TEXTURE_MIN_FILTER, GL_LINEAR},
        {GL_TEXTURE_MAG_FILTER, GL_LINEAR}
    };

    m_texture = std::make_unique<Texture2D>("Images/mountains.jpg", textureParametersClampToBorder);
    m_paperTexture = std::make_unique<Texture2D>("Images/SeamlessPaperTexture.jpg", textureParametersRepeat);

    m_framebufferA = std::make_unique<Framebuffer>(m_windowSize, GL_RGBA32F, GL_RGBA, GL_FLOAT, textureParametersNoMipmapClampToBorder);
    m_framebufferB = std::make_unique<Framebuffer>(m_windowSize, GL_RGBA32F, GL_RGBA, GL_FLOAT, textureParametersNoMipmapClampToBorder);
    m_paperTextureGradient = std::make_unique<Framebuffer>(m_paperTexture->GetSize(), GL_RGBA32F, GL_RGBA, GL_FLOAT, textureParametersNoMipmapRepeat);

    // Set up imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(400, 300));

    // Generate gradient texture
    glViewport(0, 0, m_paperTexture->GetSize().x, m_paperTexture->GetSize().y);
    glUseProgram(m_gradientProgram);
    const GLint gradientTextureUniformLocation = glGetUniformLocation(m_gradientProgram, "myTexture");
    glUniform1i(gradientTextureUniformLocation, 0);
    m_paperTexture->Bind(0); // Read Texture
    m_paperTextureGradient->Bind(); // Write Framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_fullscreenQuad->Draw();

    glViewport(0, 0, m_windowSize.x, m_windowSize.y);

    return true;
}

void Application::Tick(double deltaTime)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Settings");
    ImGui::InputInt("Spatial Radius", &m_spatialRadius, 1, 5);
    ImGui::InputFloat("Color Radius", &m_colorRadius, 0.125, 1);
    ImGui::InputInt("Iteration Count", &m_iterationCount, 1, 5);
    ImGui::InputFloat2("Wobble Magnitude", &m_wobbleMagnitude[0]);
    ImGui::InputFloat2("Gradient Offset", &m_wobbleOffset[0]);
    ImGui::InputFloat2("Wobble Texture Scale", &m_wobbleTextureScale[0]);
    ImGui::End();

    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);

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
    const GLint meanShiftSpatialRadiusUniformLocation = glGetUniformLocation(m_meanShiftProgram, "spatialRadius");
    glUniform1i(meanShiftSpatialRadiusUniformLocation, m_spatialRadius);
    const GLint meanShiftColorRadiusUniformLocation = glGetUniformLocation(m_meanShiftProgram, "colorRadius");
    glUniform1f(meanShiftColorRadiusUniformLocation, m_colorRadius);
    const GLint meanShiftIterationCountUniformLocation = glGetUniformLocation(m_meanShiftProgram, "iterationCount");
    glUniform1i(meanShiftIterationCountUniformLocation, m_iterationCount);
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
    m_framebufferA->Bind(); // Write framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_fullscreenQuad->Draw();

    /* Uncomment to visualize paper gradient
    glUseProgram(m_defaultProgram);
    const GLint defaultTextureUniformLocation = glGetUniformLocation(m_defaultProgram, "myTexture");
    glUniform1i(defaultTextureUniformLocation, 0);
    if (m_paperTextureGradient->GetColorTexture().expired()) {
        std::cerr << "Failed to get color texture!\n";
        return;
    }
    m_paperTextureGradient->GetColorTexture().lock()->Bind(0);
    Framebuffer::Unbind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_fullscreenQuad->Draw();
    */

    glUseProgram(m_wobbleProgram);
    const GLint wobbleTextureUniformLocation = glGetUniformLocation(m_wobbleProgram, "myTexture");
    glUniform1i(wobbleTextureUniformLocation, 0);
    const GLint wobbleGradientTextureUniformLocation = glGetUniformLocation(m_wobbleProgram, "gradientTexture");
    glUniform1i(wobbleGradientTextureUniformLocation, 1);
    const GLint wobbleMagnitudeUniformLocation = glGetUniformLocation(m_wobbleProgram, "wobbleMagnitude");
    glUniform2f(wobbleMagnitudeUniformLocation, m_wobbleMagnitude.x, m_wobbleMagnitude.y);
    const GLint gradientOffsetUniformLocation = glGetUniformLocation(m_wobbleProgram, "gradientOffset");
    glUniform2f(gradientOffsetUniformLocation, m_wobbleOffset.x, m_wobbleOffset.y);
    const GLint wobbleTextureScaleUniformLocation = glGetUniformLocation(m_wobbleProgram, "wobbleTextureScale");
    glUniform2f(wobbleTextureScaleUniformLocation, m_wobbleTextureScale.x, m_wobbleTextureScale.y);
    if (m_framebufferA->GetColorTexture().expired()) {
        std::cerr << "Failed to get color texture!\n";
        return;
    }
    m_framebufferA->GetColorTexture().lock()->Bind(0);
    if (m_paperTextureGradient->GetColorTexture().expired()) {
        std::cerr << "Failed to get color texture!\n";
        return;
    }
    m_paperTextureGradient->GetColorTexture().lock()->Bind(1);
    Framebuffer::Unbind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_fullscreenQuad->Draw();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}