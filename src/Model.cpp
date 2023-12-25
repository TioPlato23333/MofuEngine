#include "Model.h"

#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <stb_image.h>

Model::Model(bool gamma) : gamma_correction_(gamma) {}

void Model::Draw(Shader& shader, bool use_material) {
    glUniform1i(glGetUniformLocation(shader.Id(), "use_material"), use_material);
    for (unsigned int i = 0; i < meshes_.size(); i++) {
        meshes_[i].Draw(shader);
    }
}

void Model::SetFixedTexturePath(const std::string& path) {
    fixed_tex_path_ = path;
}

void Model::LoadModel(std::string const& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate |
        aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }
    directory_ = path.substr(0, path.find_last_of('\\'));

    ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes_.push_back(ProcessMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene);
    }
}


std::shared_ptr<Material> Model::LoadMaterial(aiMaterial* mat) {
    std::shared_ptr<Material> material = std::make_shared<Material>();
    material->shininess = 0.0f;
    material->opacity = 0.0f;
    material->density = 0.0f;
    material->illum = 0.0f;
    material->ambient = glm::vec3(0.0f);
    material->diffuse = glm::vec3(0.0f);
    material->specular = glm::vec3(0.0f);
    aiColor3D color(0.0f, 0.0f, 0.0f);
    float value = 0.0f;

    if (mat->Get(AI_MATKEY_SHININESS, value) == AI_SUCCESS) {
        material->shininess = value;
    }

    if (mat->Get(AI_MATKEY_OPACITY, value) == AI_SUCCESS) {
        material->opacity = value;
    }

    // TODO: load color transparent (it should be aiColor3D instead of float)
    // if (mat->Get(AI_MATKEY_COLOR_TRANSPARENT, value) == AI_SUCCESS) {
    //     material->density = value;
    // }

    if (mat->Get(AI_MATKEY_SHADING_MODEL, value) == AI_SUCCESS) {
        material->illum = value;
    }

    if (mat->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) {
        material->ambient = glm::vec3(color.r, color.g, color.b);
    }

    if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
        material->diffuse = glm::vec3(color.r, color.g, color.b);
    }

    if (mat->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
        material->specular = glm::vec3(color.r, color.g, color.b);
    }

    return material;
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures = {};
    std::shared_ptr<Material> material = nullptr;

    // process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        glm::vec3 vector;

        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;

        if (mesh->HasNormals()) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        }

        if (mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.tex_coords = vec;

            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.tangent = vector;

            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.bitangent = vector;
        }
        else {
            vertex.tex_coords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // process materials
    aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
    if (mat) {
        material = LoadMaterial(mat);
    }

    if (fixed_tex_path_.empty()) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuse_maps = LoadTextures(material, aiTextureType_DIFFUSE,
            "texture_diffuse", scene);
        textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

        std::vector<Texture> specular_maps = LoadTextures(material, aiTextureType_SPECULAR,
            "texture_specular", scene);
        textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());

        std::vector<Texture> normal_maps = LoadTextures(material, aiTextureType_HEIGHT,
            "texture_normal", scene);
        textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());

        std::vector<Texture> height_maps = LoadTextures(material, aiTextureType_AMBIENT,
            "texture_height", scene);
        textures.insert(textures.end(), height_maps.begin(), height_maps.end());
    }
    else {
        Texture texture;
        texture.id = TextureFromFile(fixed_tex_path_.c_str(), "", gamma_correction_);
        texture.type = "texture_diffuse";
        texture.path = fixed_tex_path_.c_str();
        textures.push_back(texture);
    }

    return Mesh(vertices, indices, textures, std::move(material));
}

std::vector<Texture> Model::LoadTextures(aiMaterial* mat, aiTextureType type,
    const std::string& type_name, const aiScene* scene) {
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        if (mat->GetTexture(type, i, &str) != AI_SUCCESS) {
            std::cout << "Failed to find texture path." <<  std::endl;
            break;
        }
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded_.size(); j++) {
            if (std::strcmp(textures_loaded_[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(textures_loaded_[j]);
                skip = true;
                break;
            }
        }
        if (!skip) {
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), directory_, gamma_correction_);
            texture.type = type_name;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded_.push_back(texture);
        }
    }
    return textures;
}

unsigned int Model::TextureFromData(void* data, int width, int height, int component_num) {
    GLenum format = GL_RGBA;
    if (component_num == 1) {
        format = GL_RED;
    } else if (component_num == 3) {
        format = GL_RGB;
    } else if (component_num == 4) {
        format = GL_RGBA;
    }
    else {
        std::cout << "Wrong component number." << std::endl;
        return 0;
    }
    GLint level = format;

    unsigned int texture_id;
    glGenTextures(1, &texture_id);

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, level, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texture_id;
}

unsigned int Model::TextureFromFile(const char* path, const std::string& directory, bool gamma) {
    std::string filename = std::string(path);
    filename = !directory.empty() ? directory + '\\' + filename : filename;

    int width = 0;
    int height = 0;
    int component_num = 0;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &component_num, 0);
    unsigned int texture_id = 0;
    if (data) {
        texture_id = TextureFromData(data, width, height, component_num);
        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return texture_id;
}
