#pragma once

#include <Base/Core.h>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include "Renderer/Material.h"
#include "Renderer/Light.h"

using GLenum = unsigned int;

class Shader
{
public:
    Shader(const std::string& filepath);
    Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
    ~Shader();
    
    void Bind();
    void UnBind();

    const std::string& GetName() const { return m_Name; }

    void UploadUniformInt(const std::string &name, int value);

    void UploadUniformFloat(const std::string &name, float value);
    void UploadUniformFloat2(const std::string &name, const glm::vec2 &value);
    void UploadUniformFloat3(const std::string &name, const glm::vec3 &value);
    void UploadUniformFloat4(const std::string &name, const glm::vec4 &value);

    void UploadUniformMat3(const std::string &name, const glm::mat3 &matrix);
    void UploadUniformMat4(const std::string &name, const glm::mat4 &matrix);

    void UploadMaterial(const Material &material);
    void UploadPointLight(int index, const PointLight &pointLight);
    void UploadDirLight(const DirLight &DirLight);
private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);
private:
    uint32_t m_RendererID;
    std::string m_Name;
};

class ShaderLibrary
{
public:
    void Add(const std::string& name, const Ref<Shader>& shader);
    void Add(const Ref<Shader>& shader);
    Ref<Shader> Load(const std::string& filepath);
    Ref<Shader> Load(const std::string& name, const std::string& filepath);

    Ref<Shader> Get(const std::string& name);

    bool Exists(const std::string& name) const;
private:
    std::unordered_map<std::string, Ref<Shader>> m_Shaders;
};