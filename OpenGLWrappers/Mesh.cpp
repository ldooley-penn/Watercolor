#include "Mesh.h"

#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

Mesh::Mesh(const std::string &filepath):
    m_modelMatrix(glm::mat4(1.0f)),
    m_vbo(0),
    m_vao(0),
    m_ebo(0),
    m_indexCount(0)
{
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str());

    if (!warn.empty()) {
        std::cout << "WARN: " << warn << std::endl;
    }
    if (!err.empty()) {
        std::cerr << "ERR: " << err << std::endl;
    }
    if (!ret) {
        return;
    }

    // Credit for unique indexing technique to https://vulkan-tutorial.com/Loading_models
    std::unordered_map<HashableVertex, uint32_t> uniqueVertices{};

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            HashableVertex hashableVertex{};

            hashableVertex.m_position.x = attrib.vertices[3 * index.vertex_index + 0] / 100.f;
            hashableVertex.m_position.y = attrib.vertices[3 * index.vertex_index + 2] / 100.f;
            hashableVertex.m_position.z = attrib.vertices[3 * index.vertex_index + 1] / 100.f;

            if (index.normal_index >= 0) {
                hashableVertex.m_normal.x = attrib.normals[3 * index.normal_index + 0];
                hashableVertex.m_normal.y = attrib.normals[3 * index.normal_index + 2];
                hashableVertex.m_normal.z = attrib.normals[3 * index.normal_index + 1];
            }
            else {
                hashableVertex.m_normal = glm::vec3(0, 0, 0);
            }

            hashableVertex.m_uv = glm::vec2(0, 0);

            if (!uniqueVertices.contains(hashableVertex)) {
                uniqueVertices[hashableVertex] = static_cast<uint32_t>(vertices.size()) / 6;

                vertices.push_back(hashableVertex.m_position.x);
                vertices.push_back(hashableVertex.m_position.y);
                vertices.push_back(hashableVertex.m_position.z);
                vertices.push_back(hashableVertex.m_normal.x);
                vertices.push_back(hashableVertex.m_normal.y);
                vertices.push_back(hashableVertex.m_normal.z);
            }

            indices.push_back(uniqueVertices[hashableVertex]);
        }
    }

    Construct(vertices, indices);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_ebo);
    glDeleteBuffers(1, &m_vbo);
}

void Mesh::Draw() const {
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::Construct(const std::vector<GLfloat>& vertexData,
                     const std::vector<unsigned int> &indices)
{
    m_indexCount = indices.size();

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(GLfloat), vertexData.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}