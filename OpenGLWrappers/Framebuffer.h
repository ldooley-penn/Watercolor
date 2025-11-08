#pragma once

#include <glad/glad.h>

class Framebuffer {
    public:
    Framebuffer();
    ~Framebuffer();

    void Bind();
    void Unbind();

    void Resize(int width, int height);

private:

};

