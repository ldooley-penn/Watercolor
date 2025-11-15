#include "Framebuffer.h"

#include <iostream>

#include "Texture2D.h"
#include "../Utils/Debug.h"

Framebuffer::Framebuffer(glm::ivec2 resolution, GLint colorTextureInternalFormat, GLint colorTextureFormat, GLint colorTextureType):
    m_resolution(resolution),
    m_colorTextureInternalFormat(colorTextureInternalFormat),
    m_colorTextureFormat(colorTextureFormat),
    m_colorTextureType(colorTextureType),
    m_fbo(0),
    m_colorTexture(nullptr),
    m_colorTextureSlot(0)
{
    Generate();
}

Framebuffer::~Framebuffer() {
    Destroy();
}

void Framebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void Framebuffer::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Resize(glm::ivec2 resolution)
{
    m_resolution = resolution;
    Destroy();
    Generate();
}

void Framebuffer::Destroy() const {
    glDeleteFramebuffers(1, &m_fbo);
}

void Framebuffer::Generate()
{
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    std::vector<TextureParameter> textureParameters = {
        {GL_TEXTURE_MIN_FILTER, GL_LINEAR},
        {GL_TEXTURE_MAG_FILTER, GL_LINEAR},
        {GL_TEXTURE_WRAP_S, GL_REPEAT},
        {GL_TEXTURE_WRAP_T, GL_REPEAT}
    };
    m_colorTexture = std::make_shared<Texture2D>(m_resolution.x, m_resolution.y, m_colorTextureInternalFormat, m_colorTextureFormat, m_colorTextureType, textureParameters);
    m_colorTexture->Bind();

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture->GetTextureID(), 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete!\n";
    }

    m_colorTexture->Unbind();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
