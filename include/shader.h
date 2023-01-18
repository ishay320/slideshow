#ifndef SHADER_H
#define SHADER_H

#include <glm/glm.hpp>
#include <string>

class Shader
{
public:
    Shader() = default;
    Shader(const std::string& vertex_shader_path, const std::string& fragment_shader_path);

    void use();

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;

    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;

    void setMat4(const std::string& name, const glm::mat4& value) const;

protected:
    int _ID;
};

#endif // SHADER_H