#include "Renderer/Shader.h"
#include "Base/Core.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <array>
#include <fstream>
#include <sstream>

static GLenum ShaderTypeFromString(const std::string& type)
{
    if (type == "vertex")
        return GL_VERTEX_SHADER;
    if (type == "fragment" || type == "pixel")
        return GL_FRAGMENT_SHADER;

    CORE_ASSERT(false, "Unknown shader type!");
    return 0;
}

Shader::Shader(const std::string& filepath)
{
    std::string source = ReadFile(filepath);
    auto shaderSources = PreProcess(source);
    Compile(shaderSources);

    // Extract name from filepath
    auto lastSlash = filepath.find_last_of("/\\");
    lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
    auto lastDot = filepath.rfind('.');
    auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
    m_Name = filepath.substr(lastSlash, count);
}

Shader::Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
    : m_Name(name)
{
    std::unordered_map<GLenum, std::string> sources;
    sources[GL_VERTEX_SHADER] = vertexSrc;
    sources[GL_FRAGMENT_SHADER] = fragmentSrc;
    Compile(sources);
}

std::string Shader::ReadFile(const std::string& filepath)
{
    std::string result;
    std::ifstream in(filepath, std::ios::in | std::ios::binary);
    if (in)
    {
        in.seekg(0, std::ios::end);
        result.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&result[0], result.size());
        in.close();
;		}
    else
    {
        LOG_ERROR("Could not open file '{0}'", filepath);
    }

    return result;
}

std::unordered_map<GLenum, std::string> Shader::PreProcess(const std::string& source)
{
    std::unordered_map<GLenum, std::string> shaderSources;

    const char* typeToken = "#type";
    size_t typeTokenLength = strlen(typeToken);
    size_t pos = source.find(typeToken, 0);
    while (pos != std::string::npos)
    {
        size_t eol = source.find_first_of("\r\n", pos);
        CORE_ASSERT(eol != std::string::npos, "Syntax error");
        size_t begin = pos + typeTokenLength + 1;
        std::string type = source.substr(begin, eol - begin);
        CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

        size_t nextLinePos = source.find_first_not_of("\r\n", eol);
        pos = source.find(typeToken, nextLinePos);
        shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
    }

    return shaderSources;
}

void Shader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
{
    GLuint program = glCreateProgram();
    CORE_ASSERT(shaderSources.size() <= 2, "We only support 2 shaders for now");
    std::array<GLenum, 2> glShaderIDs;
    int glShaderIDIndex = 0;
    for (auto& kv : shaderSources)
    {
        GLenum type = kv.first;
        const std::string& source = kv.second;

        GLuint shader = glCreateShader(type);

        const GLchar* sourceCStr = source.c_str();
        glShaderSource(shader, 1, &sourceCStr, 0);

        glCompileShader(shader);

        GLint isCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

            glDeleteShader(shader);

            LOG_ERROR("{0}", infoLog.data());
            CORE_ASSERT(false, "Shader compilation failure!");
            break;
        }

        glAttachShader(program, shader);
        glShaderIDs[glShaderIDIndex++] = shader;
    }
    
    m_RendererID = program;

    // Link our program
    glLinkProgram(program);

    // Note the different functions here: glGetProgram* instead of glGetShader*.
    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        // We don't need the program anymore.
        glDeleteProgram(program);
        
        for (auto id : glShaderIDs)
            glDeleteShader(id);

        LOG_ERROR("{0}", infoLog.data());
        CORE_ASSERT(false, "Shader link failure!");
        return;
    }

    for (auto id : glShaderIDs)
        glDetachShader(program, id);
}

Shader::~Shader() 
{
    glDeleteProgram(m_RendererID);
}

void Shader::Bind() 
{
    glUseProgram(m_RendererID);
}

void Shader::UnBind() 
{
    glUseProgram(0);
}

void Shader::UploadUniformInt(const std::string &name, int value) 
{
    int location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform1i(location, value);
}

void Shader::UploadUniformFloat(const std::string &name, float value) 
{
    int location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform1f(location, value);
}

void Shader::UploadUniformFloat2(const std::string &name, const glm::vec2 &value) 
{
    int location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform2f(location, value.x, value.y);
}

void Shader::UploadUniformFloat3(const std::string &name, const glm::vec3 &value) 
{
    int location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform3f(location, value.x, value.y, value.z);
}

void Shader::UploadUniformFloat4(const std::string &name, const glm::vec4 &value) 
{
    int location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform4f(location, value.x, value.y, value.z, value.w);
}

void Shader::UploadUniformMat3(const std::string &name, const glm::mat3 &matrix) 
{
    int location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::UploadUniformMat4(const std::string &name, const glm::mat4 &matrix) 
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::UploadMaterial(const Material &material) 
{
    UploadUniformFloat3("u_Material.Ambient", material.Ambient);
    UploadUniformFloat3("u_Material.Diffuse", material.Diffuse);
    UploadUniformFloat3("u_Material.Specular", material.Specular);
    UploadUniformFloat("u_Material.Shininess", material.Shininess);
}

void Shader::UploadPointLight(int index, const PointLight &pointLight) 
{
    std::stringstream name;
    name << "u_PointLights[" << index << "]";
    // std::string test = name.str();
    UploadUniformFloat3(name.str() + ".Position", pointLight.Position);
    UploadUniformFloat3(name.str() + ".Ambient", pointLight.Ambient);
    UploadUniformFloat3(name.str() + ".Diffuse", pointLight.Diffuse);
    UploadUniformFloat3(name.str() + ".Specular", pointLight.Specular);
    UploadUniformFloat(name.str() + ".Constant", pointLight.Constant);
    UploadUniformFloat(name.str() + ".Linear", pointLight.Linear);
    UploadUniformFloat(name.str() + ".Quadratic", pointLight.Quadratic);
}

void Shader::UploadDirLight(const DirLight &DirLight) 
{
    UploadUniformFloat3("u_DirLight.Direction", DirLight.Direction);
    UploadUniformFloat3("u_DirLight.Ambient", DirLight.Ambient);
    UploadUniformFloat3("u_DirLight.Diffuse", DirLight.Diffuse);
    UploadUniformFloat3("u_DirLight.Specular", DirLight.Specular);
}
