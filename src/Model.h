#ifndef SRC_MODEL_
#define SRC_MODEL_

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
    void Draw(Shader& shader);
    void SetFixedTexturePath(const std::string& path);
    static unsigned int TextureFromFile(const char* path, const std::string& directory,
        bool gamma = false);

private:
    void ProcessNode(aiNode* node, const aiScene* scene);
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type,
        const std::string& type_name);

    bool gamma_correction_ = false;
    std::vector<Texture> textures_loaded_ = {};
    std::vector<Mesh> meshes_ = {};
    std::string directory_;
    std::string fixed_tex_path_;
};

#endif  // SRC_MODEL_
