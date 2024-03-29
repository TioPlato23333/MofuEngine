#ifndef SRC_SHADER_H_
#define SRC_SHADER_H_

#include <string>

#include <glm/glm.hpp>

class Shader
{
public:
    Shader(const char* vertex_path, const char* fragment_path, const char* geometry_path = nullptr);
    ~Shader() = default;

    void Use();
    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetVec2(const std::string& name, const glm::vec2& value) const;
    void SetVec2(const std::string& name, float x, float y) const;
    void SetVec3(const std::string& name, const glm::vec3& value) const;
    void SetVec3(const std::string& name, float x, float y, float z) const;
    void SetVec4(const std::string& name, const glm::vec4& value) const;
    void SetVec4(const std::string& name, float x, float y, float z, float w);
    void SetMat2(const std::string& name, const glm::mat2& mat) const;
    void SetMat3(const std::string& name, const glm::mat3& mat) const;
    void SetMat4(const std::string& name, const glm::mat4& mat) const;
    inline unsigned int Id() const;

private:
    void CheckCompileErrors(unsigned int shader, std::string type);

    unsigned int id_ = 0;
};

unsigned int Shader::Id() const {
    return id_;
}

#endif  // SRC_SHADER_H_
