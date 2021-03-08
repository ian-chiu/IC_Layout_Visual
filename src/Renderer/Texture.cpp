#include "Renderer/Texture.h"
#include "stb_image.h"
#include "Base/Core.h"
#include <glad/glad.h>

Texture::Texture(const std::string &path) :
    m_RendererID(0),
    m_FilePath(path),
    m_LocalBuffer(nullptr),
    m_Width(0),
    m_Height(0),
    m_BPP(0)
{   
    // stbi_set_flip_vertically_on_load(1);
    m_LocalBuffer = stbi_load(m_FilePath.c_str(), &m_Width, &m_Height, &m_BPP, 0);
    CORE_ASSERT(m_LocalBuffer, "Falied to load texture!");

    glGenTextures(1, &m_RendererID);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_LocalBuffer);
    glGenerateMipmap(GL_TEXTURE_2D);

    if (m_LocalBuffer)
        stbi_image_free(m_LocalBuffer);
}

Texture::~Texture() 
{
    glDeleteTextures(1, &m_RendererID);
}

void Texture::Bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Texture::UnBind() 
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
