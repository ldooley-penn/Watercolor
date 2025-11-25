#pragma once
#include <string>

#include "glad/glad.h"
#include "glm/glm.hpp"

struct aiMesh;
struct aiNode;
struct aiScene;

class Mesh {
public:
    explicit Mesh(const std::string& filepath);
    ~Mesh();

    [[nodiscard]] glm::mat4 GetModelMatrix() const {
        return m_modelMatrix;
    }
    void Draw() const;

private:
    void Construct(const std::vector<GLfloat>& vertexData, const std::vector<unsigned int>& indices);

    glm::mat4 m_modelMatrix;
    GLuint m_vbo;
    GLuint m_vao;
    GLuint m_ebo;

    int m_indexCount;
};
