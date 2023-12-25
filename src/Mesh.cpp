#include "Mesh.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <GL/glew.h>

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices,
    const std::vector<Texture>& textures) {
    vertices_ = vertices;
    indices_ = indices;
    textures_ = textures;

    SetupMesh();
}

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices,
    const std::vector<Texture>& textures, std::shared_ptr<Material>&& material) {
    vertices_ = vertices;
    indices_ = indices;
    textures_ = textures;
    material_ = material;

    SetupMesh();
}

void Mesh::Draw(Shader& shader) {
    unsigned int diffuse_index = 1;
    unsigned int specular_index = 1;
    unsigned int normal_index = 1;
    unsigned int height_index = 1;
    for (unsigned int i = 0; i < textures_.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string number;
        std::string name = textures_[i].type;
        if (name == "texture_diffuse") {
            number = std::to_string(diffuse_index++);
        } else if (name == "texture_specular") {
            number = std::to_string(specular_index++);
        } else if (name == "texture_normal") {
            number = std::to_string(normal_index++);
        } else if (name == "texture_height") {
            number = std::to_string(height_index++);
        }
        glUniform1i(glGetUniformLocation(shader.Id(), (name + number).c_str()), i);
    }
    if (material_) {
        glUniform1f(glGetUniformLocation(shader.Id(), "material.shininess"),
            material_->shininess);
        glUniform1f(glGetUniformLocation(shader.Id(), "material.opacity"),
            material_->opacity);
        glUniform1f(glGetUniformLocation(shader.Id(), "material.density"),
            material_->density);
        glUniform1f(glGetUniformLocation(shader.Id(), "material.illum"),
            material_->illum);
        glUniform3f(glGetUniformLocation(shader.Id(), "material.ambient"),
            material_->ambient[0], material_->ambient[1], material_->ambient[2]);
        glUniform3f(glGetUniformLocation(shader.Id(), "material.diffuse"),
            material_->diffuse[0], material_->diffuse[1], material_->diffuse[2]);
        glUniform3f(glGetUniformLocation(shader.Id(), "material.specular"),
            material_->specular[0], material_->specular[1], material_->specular[2]);
        glUniform3f(glGetUniformLocation(shader.Id(), "material.ambient"),
            material_->ambient[0], material_->ambient[1], material_->ambient[2]);
        glUniform3f(glGetUniformLocation(shader.Id(), "material.diffuse"),
            material_->diffuse[0], material_->diffuse[1], material_->diffuse[2]);
        glUniform3f(glGetUniformLocation(shader.Id(), "material.specular"),
            material_->specular[0], material_->specular[1], material_->specular[2]);

        // TODO: deal with light
        glm::vec3 const_one(1.0f);
        glUniform3f(glGetUniformLocation(shader.Id(), "light.ambient"),
            const_one[0], const_one[1], const_one[2]);
        glUniform3f(glGetUniformLocation(shader.Id(), "light.diffuse"),
            const_one[0], const_one[1], const_one[2]);
        glUniform3f(glGetUniformLocation(shader.Id(), "light.specular"),
            const_one[0], const_one[1], const_one[2]);
    }

    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices_.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}

void Mesh::SetupMesh() {
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), &vertices_[0],
        GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int),
        &indices_[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, normal)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, tex_coords)));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, tangent)));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, bitangent)));

    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, bone_ids)));

    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, weights)));
    glBindVertexArray(0);
}
