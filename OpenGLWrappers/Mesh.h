#pragma once
#include <string>

#include "glad/glad.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <unordered_set>
#include <glm/gtx/hash.hpp>
#include "glm/glm.hpp"

struct aiMesh;
struct aiNode;
struct aiScene;

// Credit to https://vulkan-tutorial.com/Loading_models

struct HashableVertex {
    glm::vec3 m_position;
    glm::vec3 m_normal;
    glm::vec2 m_uv;

    bool operator==(const HashableVertex& other) const {
        return m_position == other.m_position && m_normal == other.m_normal && m_uv == other.m_uv;
    }


};

namespace std {
    template<> struct hash<HashableVertex> {
        size_t operator()(HashableVertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.m_position) ^
                   (hash<glm::vec3>()(vertex.m_normal) << 1)) >> 1) ^
                   (hash<glm::vec2>()(vertex.m_uv) << 1);
        }
    };
}

class Mesh {
public:
    explicit Mesh(const std::string& filepath);
    ~Mesh();

    [[nodiscard]] glm::mat4 GetModelMatrix() const {
        return m_modelMatrix;
    }
    void Draw() const;

private:
    static std::unordered_map<GLuint, std::unordered_set<GLuint>> GenerateNeighborMap(const std::vector<GLuint>& indices);

    static void CalculateVertexNormals(std::vector<GLfloat>& vertexData, const std::vector<GLuint>& indices);

    static void SmoothNormals(unsigned int iterations, std::vector<GLfloat>& vertexData, const std::vector<GLuint>& indices);

    void Construct(const std::vector<GLfloat>& vertexData, const std::vector<GLuint>& indices);

    glm::mat4 m_modelMatrix;
    GLuint m_vbo;
    GLuint m_vao;
    GLuint m_ebo;

    int m_indexCount;
};
