#ifndef SRC_MODEL_H_
#define SRC_MODEL_H_

#include <string>
#include <vector>

#include <assimp/scene.h>

#include "Mesh.h"
#include "Shader.h"

class Model {
public:
    Model(bool gamma = false);
    ~Model() = default;

    void LoadModel(std::string const& path);
    void Draw(Shader& shader, bool use_material);
    void SetFixedTexturePath(const std::string& path);

private:
    void ProcessNode(aiNode* node, const aiScene* scene);
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
    std::shared_ptr<Material> LoadMaterial(aiMaterial* mat);
    std::vector<Texture> LoadTextures(aiMaterial* mat, aiTextureType type,
        const std::string& type_name, const aiScene* scene);
    unsigned int TextureFromData(void* data, int width, int height,
        int component_num);
    unsigned int TextureFromFile(const char* path, const std::string& directory,
        bool gamma);

    bool gamma_correction_ = false;
    std::vector<Texture> textures_loaded_ = {};
    std::vector<Mesh> meshes_ = {};
    std::string directory_;
    std::string fixed_tex_path_;
};

#endif  // SRC_MODEL_H_
