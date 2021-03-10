#include "Renderer/Buffer.h"
#include <glad/glad.h>

VertexBuffer::VertexBuffer(float *vertices, unsigned int size) 
{
    glCreateBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

// VertexBuffer::VertexBuffer(const std::vector<float> &vertices) 
// {
//     glCreateBuffers(1, &m_RendererID);
//     glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
//     glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices, GL_STATIC_DRAW);
// }

VertexBuffer::~VertexBuffer() 
{
    glDeleteBuffers(1, &m_RendererID);
}

void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void VertexBuffer::UnBind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//=================================================================

IndexBuffer::IndexBuffer(unsigned int *indices, unsigned int count) 
    : m_Count(count)
{
    glCreateBuffers(1, &m_RendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * count, indices, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer() 
{
    glDeleteBuffers(1, &m_RendererID);
}

void IndexBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void IndexBuffer::UnBind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
