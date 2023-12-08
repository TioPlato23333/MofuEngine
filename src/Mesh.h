#ifndef SRC_MESH_
#define SRC_MESH_

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "Shader.h"

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

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices,
        const std::vector<Texture>& textures);
    ~Mesh() = default;

    void Draw(Shader& shader);

private:
    void SetupMesh();

    unsigned int vao_ = 0;
    unsigned int vbo_ = 0;
    unsigned int ebo_ = 0;
    std::vector<Vertex> vertices_ = {};
    std::vector<unsigned int> indices_ = {};
    std::vector<Texture> textures_ = {};
};

#endif  // SRC_MESH_
