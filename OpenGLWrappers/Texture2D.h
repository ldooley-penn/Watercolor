#pragma once

#include <string>
#include <vector>
#include <glad/glad.h>

struct TextureParameter {
    GLenum pname;
    GLint value;
};

class Texture2D {
public:
    explicit Texture2D(const std::string& imageFilepath, const std::vector<TextureParameter>& textureParameters);
    explicit Texture2D(int width, int height, GLint textureFormat, const std::vector<TextureParameter>& textureParameters);
    ~Texture2D();

    void Bind(int TextureSlot = 0);
    void Unbind() const;

    [[nodiscard]] GLuint GetTextureID() const {
        return m_texture;
    }

private:
    GLuint m_texture;

    int m_width;
    int m_height;
    GLint m_internalFormat;

    GLenum m_textureSlot;
};
