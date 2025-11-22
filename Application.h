#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <array>
#include <memory>
#include <unordered_set>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include "OpenGLWrappers/Texture2D.h"

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

    void ApplyWaterColorEffects(const std::unique_ptr<Framebuffer>& baseColorFramebuffer) const;

    void WobbleEdges(const std::unique_ptr<Framebuffer>& source, const std::unique_ptr<Framebuffer>& destination) const;

    void DarkenEdges(const std::unique_ptr<Framebuffer>& source, const std::unique_ptr<Framebuffer>& destination) const;

    void ApplyTurbulentFlow(const std::unique_ptr<Framebuffer>& source, const std::unique_ptr<Framebuffer>& destination) const;

    void UpdateMeanShiftedImage();

    GLFWwindow* m_window;
    GLuint m_defaultProgram;
    GLuint m_rgbToLuvProgram;
    GLuint m_luvToRgbProgram;
    GLuint m_meanShiftProgram;
    GLuint m_gradientProgram;
    GLuint m_wobbleProgram;
    GLuint m_edgeDarkeningProgram;
    GLuint m_turbulentFlowProgram;

    std::unordered_set<int> m_keysPressed;

    std::unordered_set<int> m_mouseButtonsPressed;

    glm::dvec2 m_mousePosition;

    glm::ivec2 m_windowSize;

    std::unique_ptr<FullscreenQuad> m_fullscreenQuad;

    std::unique_ptr<Texture2D> m_texture;
    std::unique_ptr<Texture2D> m_paperTexture;
    std::unique_ptr<Framebuffer> m_paperTextureGradient;

    std::array<std::unique_ptr<Framebuffer>, 2> m_pingPongFramebuffers;

    std::unique_ptr<Framebuffer> m_meanShiftedImage;

    int m_spatialRadius = 5;
    float m_colorRadius = 0.75f;
    int m_iterationCount = 5;
    glm::vec2 m_wobbleMagnitude = glm::vec2(10.f);
    glm::vec2 m_wobbleOffset = glm::vec2(0.0f, -1.0f);
    glm::vec2 m_wobbleTextureScale = glm::vec2(1.0f);
    float m_edgeDarkeningMagnitude = 8.0f;
    glm::vec2 m_turbulentFlowScale = glm::vec2(100.0f);
    float m_turbulentFlowIntensity = 1.0f;

    std::vector<TextureParameter> m_imageTextureParameters;
};