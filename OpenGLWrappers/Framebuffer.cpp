#include "Framebuffer.h"

#include <iostream>

Framebuffer::Framebuffer(glm::ivec2 resolution, GLint colorTextureFormat):
    m_resolution(resolution),
    m_colorTextureFormat(colorTextureFormat),
    m_fbo(0),
    m_colorTexture(0),
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

void Framebuffer::BindColorTexture(const int TextureSlot)
{
    m_colorTextureSlot = TextureSlot;
    glActiveTexture(GL_TEXTURE0 + m_colorTextureSlot);
    glBindTexture(GL_TEXTURE_2D, m_colorTexture);
}

void Framebuffer::UnbindColorTexture() const
{
    glActiveTexture(m_colorTextureSlot);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Framebuffer::Resize(glm::ivec2 resolution)
{
    m_resolution = resolution;
    Destroy();
    Generate();
}

void Framebuffer::Destroy() const {
    glDeleteFramebuffers(1, &m_fbo);
    glDeleteTextures(1, &m_colorTexture);
}

void Framebuffer::Generate()
{
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glGenTextures(1, &m_colorTexture);
    glBindTexture(GL_TEXTURE_2D, m_colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, m_colorTextureFormat, m_resolution.x, m_resolution.y, 0, m_colorTextureFormat, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete!\n";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
