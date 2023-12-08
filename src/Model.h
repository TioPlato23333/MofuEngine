#ifndef SRC_MODEL_
#define SRC_MODEL_

#include <string>
#include <vector>

#include <assimp/scene.h>

#include "Mesh.h"
#include "Shader.h"

class Model {
public:
    bool gamma_correction_ = false;
    std::vector<Texture> textures_loaded_ = {};
    std::vector<Mesh> meshes_ = {};
    std::string directory_;

    Model(std::string const& path, bool gamma = false);
    ~Model() = default;

    void Draw(Shader& shader);
    static unsigned int TextureFromFile(const char* path, const std::string& directory,
        bool gamma = false);

private:
    void LoadModel(std::string const& path);
    void ProcessNode(aiNode* node, const aiScene* scene);
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type,
        const std::string& type_name);
};

#endif  // SRC_MODEL_
