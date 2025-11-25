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

    int i = 0;
    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                vertices.push_back(attrib.vertices[3*size_t(idx.vertex_index)+0] / 100.f);
                vertices.push_back(attrib.vertices[3*size_t(idx.vertex_index)+2] / 100.f);
                vertices.push_back(attrib.vertices[3*size_t(idx.vertex_index)+1] / 100.f);

                if (idx.normal_index >= 0) {
                    vertices.push_back(attrib.normals[3*size_t(idx.normal_index)+0]);
                    vertices.push_back(attrib.normals[3*size_t(idx.normal_index)+1]);
                    vertices.push_back(attrib.normals[3*size_t(idx.normal_index)+2]);
                }
                else {
                    vertices.push_back(0);
                    vertices.push_back(0);
                    vertices.push_back(0);
                }

                /*
                if (idx.texcoord_index >= 0) {
                    tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
                    tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
                }
                */

                indices.push_back(i);
                i++;
            }
            index_offset += fv;
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