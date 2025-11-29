#include "Application.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include "Utils/ShaderLoader.h"
#include "Camera.h"
#include "OpenGLWrappers/Mesh.h"

#include <glm/gtc/type_ptr.hpp>

#include "OpenGLWrappers/Framebuffer.h"
#include "OpenGLWrappers/FullscreenQuad.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <nfd.h>

#include "Utils/Debug.h"

Application::Application(const glm::ivec2 windowSize) :
    m_window(nullptr),
    m_defaultProgram(0),
    m_rgbToLuvProgram(0), m_luvToRgbProgram(0), m_meanShiftProgram(0), m_gradientProgram(0), m_wobbleProgram(0),
    m_edgeDarkeningProgram(0), m_pigmentVariationProgram(0), m_toonProgram(0),
    m_mousePosition(glm::dvec2(0, 0)),
    m_windowSize(windowSize),
    m_camera(std::make_unique<Camera>(m_windowSize, glm::vec3(0, 0, 2), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), 1.f, 1.f)),
    m_fullscreenQuad(nullptr),
    m_texture(nullptr),
    m_mesh(nullptr),
    m_paperTexture(nullptr),
    m_paperTextureGradient(nullptr),
    m_pingPongFramebuffers({nullptr, nullptr}),
    m_meanShiftedImage(nullptr),
    m_imageTextureParameters(std::vector<TextureParameter>())
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
    if (m_mouseButtonsPressed.contains(GLFW_MOUSE_BUTTON_RIGHT))
    {
        const glm::dvec2 mouseDelta = glm::dvec2(xpos, ypos) - m_mousePosition;
    }
    m_mousePosition = glm::dvec2(xpos, ypos);
}

void Application::UpdateFramebufferSize(const int width, const int height)
{
    m_windowSize = glm::ivec2(width, height);

    m_pingPongFramebuffers[0]->Resize(m_windowSize);
    m_pingPongFramebuffers[1]->Resize(m_windowSize);

    m_camera->Resize(m_windowSize);

    UpdateMeanShiftedImage();
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

    glEnable(GL_CULL_FACE);

    m_defaultProgram = ShaderLoader::createShaderProgram("Shaders/Default.vert", "Shaders/Default.frag");
    m_rgbToLuvProgram = ShaderLoader::createShaderProgram("Shaders/RGBtoLUV.vert", "Shaders/RGBtoLUV.frag");
    m_luvToRgbProgram = ShaderLoader::createShaderProgram("Shaders/LUVtoRGB.vert", "Shaders/LUVtoRGB.frag");
    m_meanShiftProgram = ShaderLoader::createShaderProgram("Shaders/MeanShift.vert", "Shaders/MeanShift.frag");
    m_gradientProgram = ShaderLoader::createShaderProgram("Shaders/Gradient.vert", "Shaders/Gradient.frag");
    m_wobbleProgram = ShaderLoader::createShaderProgram("Shaders/Wobble.vert", "Shaders/Wobble.frag");
    m_edgeDarkeningProgram = ShaderLoader::createShaderProgram("Shaders/EdgeDarkening.vert", "Shaders/EdgeDarkening.frag");
    m_pigmentVariationProgram = ShaderLoader::createShaderProgram("Shaders/PigmentVariation.vert", "Shaders/PigmentVariation.frag");
    m_toonProgram = ShaderLoader::createShaderProgram("Shaders/Toon.vert", "Shaders/Toon.frag");

    m_fullscreenQuad = std::make_unique<FullscreenQuad>();

    m_imageTextureParameters = {
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

    m_texture = std::make_unique<Texture2D>("Images/mountains.jpg", m_imageTextureParameters);
    m_paperTexture = std::make_unique<Texture2D>("Images/SeamlessPaperTexture.jpg", textureParametersRepeat);

    m_pingPongFramebuffers[0] = std::make_unique<Framebuffer>(m_windowSize, GL_RGBA32F, GL_RGBA, GL_FLOAT, textureParametersNoMipmapClampToBorder);
    m_pingPongFramebuffers[1] = std::make_unique<Framebuffer>(m_windowSize, GL_RGBA32F, GL_RGBA, GL_FLOAT, textureParametersNoMipmapClampToBorder);
    m_paperTextureGradient = std::make_unique<Framebuffer>(m_paperTexture->GetSize(), GL_RGBA32F, GL_RGBA, GL_FLOAT, textureParametersNoMipmapRepeat);
    m_meanShiftedImage = std::make_unique<Framebuffer>(m_texture->GetSize(), GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, textureParametersNoMipmapClampToBorder);

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

    // Generate mean shifted texture
    UpdateMeanShiftedImage();

    return true;
}

void Application::Tick(double deltaTime)
{
    glm::vec3 forwardRightUpInput = glm::vec3(0);
    if (m_keysPressed.contains(GLFW_KEY_W)) {
        forwardRightUpInput.x += 1.0;
    }
    if (m_keysPressed.contains(GLFW_KEY_S)) {
        forwardRightUpInput.x -= 1.0;
    }
    if (m_keysPressed.contains(GLFW_KEY_A)) {
        forwardRightUpInput.y -= 1.0;
    }
    if (m_keysPressed.contains(GLFW_KEY_D)) {
        forwardRightUpInput.y += 1.0;
    }
    if (m_keysPressed.contains(GLFW_KEY_SPACE)) {
        forwardRightUpInput.z += 1.0;
    }
    if (m_keysPressed.contains(GLFW_KEY_LEFT_SHIFT)) {
        forwardRightUpInput.z -= 1.0;
    }
    if (forwardRightUpInput != glm::vec3(0)) {
        m_camera->MoveCamera(glm::normalize(forwardRightUpInput), deltaTime);
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Settings");
    bool requiresMeanShiftUpdate = false;
    if (ImGui::Button("Load Image")) {
        NFD_Init();

        nfdu8char_t *outPath;
        nfdu8filteritem_t filters[1] = { { "Images", "png,jpg,jpeg" }};
        nfdopendialogu8args_t args = {0};
        args.filterList = filters;
        args.filterCount = 1;
        args.defaultPath = "Images/";
        nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);
        if (result == NFD_OKAY)
        {
            m_texture = std::make_unique<Texture2D>(outPath, m_imageTextureParameters);
            requiresMeanShiftUpdate = true;
            m_renderMode = RenderMode::Image;
            NFD_FreePathU8(outPath);
        }
        else if (result != NFD_CANCEL)
        {
            printf("Error: %s\n", NFD_GetError());
        }

        NFD_Quit();
    }
    if (ImGui::Button("Load OBJ")) {
        NFD_Init();

        nfdu8char_t *outPath;
        nfdu8filteritem_t filters[1] = { { "Meshes", "obj" }};
        nfdopendialogu8args_t args = {0};
        args.filterList = filters;
        args.filterCount = 1;
        args.defaultPath = "Meshes/";
        nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);
        if (result == NFD_OKAY)
        {
            m_mesh = std::make_unique<Mesh>(outPath);
            m_renderMode = RenderMode::Mesh;
            NFD_FreePathU8(outPath);
        }
        else if (result != NFD_CANCEL)
        {
            printf("Error: %s\n", NFD_GetError());
        }

        NFD_Quit();
    }
    if (ImGui::InputInt("Spatial Radius", &m_spatialRadius, 1, 5)) {
        requiresMeanShiftUpdate = true;
    }
    if (ImGui::InputFloat("Color Radius", &m_colorRadius, 0.125, 1)) {
        requiresMeanShiftUpdate = true;
    }
    if (ImGui::InputInt("Iteration Count", &m_iterationCount, 1, 5)) {
        requiresMeanShiftUpdate = true;
    }
    ImGui::InputFloat2("Wobble Magnitude", &m_wobbleMagnitude[0]);
    ImGui::InputFloat2("Gradient Offset", &m_wobbleOffset[0]);
    ImGui::InputFloat2("Wobble Texture Scale", &m_wobbleTextureScale[0]);
    ImGui::InputFloat("Edge Darkening Magnitude", &m_edgeDarkeningMagnitude, 0.5, 2.0);
    ImGui::InputFloat("Turbulent Flow Scale", &m_turbulentFlowScale, 1.0, 10.0);
    ImGui::InputFloat("Turbulent Flow Magnitude", &m_turbulentFlowIntensity, 0.5, 2.0);
    ImGui::InputFloat("Paper Grain Scale", &m_paperGrainScale, 0.5, 10.0);
    ImGui::InputFloat("Paper Grain Intensity", &m_paperGrainIntensity, 0.5, 2.0);
    ImGui::InputFloat("Gaussian Nosie Scale", &m_gaussianNoiseScale, 0.5, 10.0);
    ImGui::InputFloat("Gaussian Noise Intensity", &m_gaussianNoiseIntensity, 0.5, 2.0);
    ImGui::End();

    if (requiresMeanShiftUpdate) {
        UpdateMeanShiftedImage();
    }

    if (m_renderMode == RenderMode::Mesh) {
        RenderMesh(m_pingPongFramebuffers[0]);
        ApplyWaterColorEffects(m_pingPongFramebuffers[0]);
    }
    else {
        ApplyWaterColorEffects(m_meanShiftedImage);
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

void Application::ApplyWaterColorEffects(const std::unique_ptr<Framebuffer> &baseColorFramebuffer) const {
    int pingPongWriteIndex = baseColorFramebuffer == m_pingPongFramebuffers[0] ? 1 : 0;
    int pingPongReadIndex = (pingPongWriteIndex + 1) % 2;

    // Wobble edges
    WobbleEdges(baseColorFramebuffer, m_pingPongFramebuffers[pingPongWriteIndex]);

    pingPongWriteIndex = (pingPongWriteIndex + 1) % 2;
    pingPongReadIndex = (pingPongReadIndex + 1) % 2;

    // Edge darkening
    DarkenEdges(m_pingPongFramebuffers[pingPongReadIndex], m_pingPongFramebuffers[pingPongWriteIndex]);

    pingPongWriteIndex = (pingPongWriteIndex + 1) % 2;
    pingPongReadIndex = (pingPongReadIndex + 1) % 2;

    // Turbulent Flow
    ApplyPigmentVariation(m_pingPongFramebuffers[pingPongReadIndex], m_pingPongFramebuffers[pingPongWriteIndex]);
}

void Application::WobbleEdges(const std::unique_ptr<Framebuffer> &source,
    const std::unique_ptr<Framebuffer> &destination) const {
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

    source->GetColorTexture()->Bind(0);
    m_paperTextureGradient->GetColorTexture()->Bind(1);
    destination->Bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_fullscreenQuad->Draw();
}

void Application::DarkenEdges(const std::unique_ptr<Framebuffer> &source,
    const std::unique_ptr<Framebuffer> &destination) const {
    glUseProgram(m_edgeDarkeningProgram);
    const GLint edgeDarkeningTextureUniformLocation = glGetUniformLocation(m_edgeDarkeningProgram, "myTexture");
    glUniform1i(edgeDarkeningTextureUniformLocation, 0);
    const GLint edgeDarkeningEdgeDarkeningMagnitudeUniformLocation = glGetUniformLocation(m_edgeDarkeningProgram, "edgeDarkeningMagnitude");
    glUniform1f(edgeDarkeningEdgeDarkeningMagnitudeUniformLocation, m_edgeDarkeningMagnitude);

    source->GetColorTexture()->Bind(0);
    destination->Bind();

    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    glViewport(0, 0, width, height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_fullscreenQuad->Draw();
}

void Application::ApplyPigmentVariation(const std::unique_ptr<Framebuffer> &source,
    const std::unique_ptr<Framebuffer> &destination) const {
        glUseProgram(m_pigmentVariationProgram);
        const GLint myTextureUniformLocation = glGetUniformLocation(m_pigmentVariationProgram, "myTexture");
        glUniform1i(myTextureUniformLocation, 0);
        const GLint paperTextureUniformLocation = glGetUniformLocation(m_pigmentVariationProgram, "paperTexture");
        glUniform1i(paperTextureUniformLocation, 1);
        const GLint turbulentFlowScaleUniformLocation = glGetUniformLocation(m_pigmentVariationProgram, "turbulentFlowScale");
        glUniform1f(turbulentFlowScaleUniformLocation, m_turbulentFlowScale);
        const GLint turbulentFlowIntensityUniformLocation = glGetUniformLocation(m_pigmentVariationProgram, "turbulentFlowIntensity");
        glUniform1f(turbulentFlowIntensityUniformLocation, m_turbulentFlowIntensity);
        const GLint paperGrainScaleUniformLocation = glGetUniformLocation(m_pigmentVariationProgram, "paperGrainScale");
        glUniform1f(paperGrainScaleUniformLocation, m_paperGrainScale);
        const GLint paperGrainIntensityUniformLocation = glGetUniformLocation(m_pigmentVariationProgram, "paperGrainIntensity");
        glUniform1f(paperGrainIntensityUniformLocation, m_paperGrainIntensity);
        const GLint gaussianNoiseScaleUniformLocation = glGetUniformLocation(m_pigmentVariationProgram, "gaussianNoiseScale");
        glUniform1f(gaussianNoiseScaleUniformLocation, m_gaussianNoiseScale);
        const GLint gaussianNoiseIntensityUniformLocation = glGetUniformLocation(m_pigmentVariationProgram, "gaussianNoiseIntensity");
        glUniform1f(gaussianNoiseIntensityUniformLocation, m_gaussianNoiseIntensity);

        source->GetColorTexture()->Bind(0);
        m_paperTextureGradient->GetColorTexture()->Bind(1);
        Framebuffer::Unbind();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_fullscreenQuad->Draw();
}

void Application::UpdateMeanShiftedImage() {
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);

    int pingPongReadIndex = 1;
    int pingPongWriteIndex = 0;

    // Convert to LUV
    glUseProgram(m_rgbToLuvProgram);
    const GLint rgbToLuvTextureUniformLocation = glGetUniformLocation(m_rgbToLuvProgram, "myTexture");
    glUniform1i(rgbToLuvTextureUniformLocation, 0);
    m_texture->Bind(0); // Read Texture
    m_pingPongFramebuffers[pingPongWriteIndex]->Bind(); // Write Framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_fullscreenQuad->Draw();

    pingPongReadIndex = (pingPongReadIndex + 1) % 2;
    pingPongWriteIndex = (pingPongWriteIndex + 1) % 2;

    // Run mean shift
    glUseProgram(m_meanShiftProgram);
    const GLint meanShiftTextureUniformLocation = glGetUniformLocation(m_meanShiftProgram, "myTexture");
    glUniform1i(meanShiftTextureUniformLocation, 0);
    m_pingPongFramebuffers[pingPongReadIndex]->GetColorTexture()->Bind(0); // Read Texture
    const GLint meanShiftSpatialRadiusUniformLocation = glGetUniformLocation(m_meanShiftProgram, "spatialRadius");
    glUniform1i(meanShiftSpatialRadiusUniformLocation, m_spatialRadius);
    const GLint meanShiftColorRadiusUniformLocation = glGetUniformLocation(m_meanShiftProgram, "colorRadius");
    glUniform1f(meanShiftColorRadiusUniformLocation, m_colorRadius);
    const GLint meanShiftIterationCountUniformLocation = glGetUniformLocation(m_meanShiftProgram, "iterationCount");
    glUniform1i(meanShiftIterationCountUniformLocation, m_iterationCount);
    m_pingPongFramebuffers[pingPongWriteIndex]->Bind(); // Write Framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_fullscreenQuad->Draw();

    pingPongReadIndex = (pingPongReadIndex + 1) % 2;
    pingPongWriteIndex = (pingPongWriteIndex + 1) % 2;

    // Convert to RGB
    glUseProgram(m_luvToRgbProgram);
    const GLint luvToRGBTextureUniformLocation = glGetUniformLocation(m_luvToRgbProgram, "myTexture");
    glUniform1i(luvToRGBTextureUniformLocation, 0);
    m_pingPongFramebuffers[pingPongReadIndex]->GetColorTexture()->Bind(0); // Read Texture
    m_meanShiftedImage->Bind(); // Write framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_fullscreenQuad->Draw();
}

void Application::RenderMesh(const std::unique_ptr<Framebuffer> &destination) const {
    glUseProgram(m_toonProgram);
    const GLint modelMatrixUniformLocation = glGetUniformLocation(m_toonProgram, "modelMatrix");
    glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(m_mesh->GetModelMatrix()));
    const GLint viewMatrixUniformLocation = glGetUniformLocation(m_toonProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(m_camera->GetViewMatrix()));
    const GLint projectionMatrixUniformLocation = glGetUniformLocation(m_toonProgram, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjectionMatrix()));

    destination->Bind();
    glViewport(0, 0, m_windowSize.x, m_windowSize.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    m_mesh->Draw();
    glDisable(GL_DEPTH_TEST);
}
