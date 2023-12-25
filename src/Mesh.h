#ifndef SRC_MESH_H_
#define SRC_MESH_H_

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "Shader.h"
#include <assimp/scene.h>

static constexpr int MAX_BONE_INFLUENCE = 4;

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    int bone_ids[MAX_BONE_INFLUENCE];
    float weights[MAX_BONE_INFLUENCE];
};

struct Material {
    float shininess;
    float opacity;
    float density;
    float illum;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices,
        const std::vector<Texture>& textures);
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices,
        const std::vector<Texture>& textures, std::shared_ptr<Material>&& material);
    ~Mesh() = default;

    void Draw(Shader& shader);

private:
    void SetupMesh();

    unsigned int vao_ = 0;
    unsigned int vbo_ = 0;
    unsigned int ebo_ = 0;
    std::shared_ptr<Material> material_ = nullptr;
    std::vector<Vertex> vertices_ = {};
    std::vector<unsigned int> indices_ = {};
    std::vector<Texture> textures_ = {};
};

#endif  // SRC_MESH_H_
