#pragma once

#include <string>
#include <glad/glad.h>

class Texture2D {
public:
    explicit Texture2D(const std::string& imageFilepath);
    ~Texture2D();

    void Bind(int TextureSlot = 0);
    void Unbind() const;

private:
    GLuint m_texture;

    int m_width;
    int m_height;
    GLint m_internalFormat;

    GLenum m_textureSlot;
};
