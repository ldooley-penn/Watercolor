#include "Framebuffer.h"

#include <iostream>

#include "Texture2D.h"
#include "../Utils/Debug.h"

Framebuffer::Framebuffer(glm::ivec2 resolution, GLint colorTextureInternalFormat, GLint colorTextureFormat, GLint colorTextureType, const std::vector<TextureParameter>& colorTextureParameters):
    m_resolution(resolution),
    m_colorTextureInternalFormat(colorTextureInternalFormat),
    m_colorTextureFormat(colorTextureFormat),
    m_colorTextureType(colorTextureType),
    m_fbo(0),
    m_colorTexture(nullptr),
    m_colorTextureSlot(0),
    m_colorTextureParameters(colorTextureParameters)
{
    Generate();
}

Framebuffer::~Framebuffer() {
    Destroy();
}

void Framebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_resolution.x, m_resolution.y);
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

    m_colorTexture = std::make_shared<Texture2D>(m_resolution.x, m_resolution.y, m_colorTextureInternalFormat, m_colorTextureFormat, m_colorTextureType, m_colorTextureParameters);
    m_colorTexture->Bind();

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture->GetTextureID(), 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete!\n";
    }

    m_colorTexture->Unbind();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
