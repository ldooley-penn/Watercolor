//
// Created by Logan on 11/8/2025.
//

#include "Texture2D.h"

#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture2D::Texture2D(const std::string &imageFilepath, const std::vector<TextureParameter>& textureParameters):
    m_texture(0),
    m_width(0),
    m_height(0),
    m_internalFormat(GL_RGBA),
    m_textureSlot(GL_TEXTURE0)
{
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    for (const auto& textureParameter: textureParameters) {
        glTexParameteri(GL_TEXTURE_2D, textureParameter.pname, textureParameter.value);
    }

    int numChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* imageData = stbi_load(imageFilepath.c_str(), &m_width, &m_height, &numChannels, 0);
    switch (numChannels) {
        case 1:
            m_internalFormat = GL_RED;
            break;
        case 2:
            m_internalFormat = GL_RG;
            break;
        case 3:
            m_internalFormat = GL_RGB;
            break;
        case 4:
            m_internalFormat = GL_RGBA;
            break;
        default:
            std::cout << "Image at " << imageFilepath << "has an unsupported number of channels: " << numChannels << std::endl;
            break;
    }
    int bytesPerRow = m_width * numChannels;
    if (bytesPerRow % 4 != 0) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }
    else {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    }
    if (imageData)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width, m_height, 0, m_internalFormat, GL_UNSIGNED_BYTE, imageData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        const char* reason = stbi_failure_reason();
        std::cout << "Failed to load texture:"<<reason<<std::endl;
    }

    stbi_image_free(imageData);

    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture2D::Texture2D(int width, int height, GLint internalFormat, GLenum format, GLenum type, const std::vector<TextureParameter>& textureParameters):
    m_texture(0),
    m_width(width),
    m_height(height),
    m_internalFormat(internalFormat),
    m_format(format),
    m_type(type),
    m_textureSlot(GL_TEXTURE0)
{
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width, m_height, 0, m_format, m_type, nullptr);

    for (const auto& textureParameter: textureParameters) {
        glTexParameteri(GL_TEXTURE_2D, textureParameter.pname, textureParameter.value);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture2D::~Texture2D() {
    glDeleteTextures(1, &m_texture);
}

void Texture2D::Bind(const int TextureSlot) {
    m_textureSlot = GL_TEXTURE0 + TextureSlot;

    glActiveTexture(m_textureSlot);
    glBindTexture(GL_TEXTURE_2D, m_texture);
}

void Texture2D::Unbind() const {
    glActiveTexture(m_textureSlot);
    glBindTexture(GL_TEXTURE_2D, 0);
}
