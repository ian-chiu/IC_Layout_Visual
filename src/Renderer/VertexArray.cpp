#include "VertexArray.h"
#include <glad/glad.h>

static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
{
    switch(type)
    {
        case ShaderDataType::Float:         return GL_FLOAT;
        case ShaderDataType::Float2:        return GL_FLOAT;
        case ShaderDataType::Float3:        return GL_FLOAT;
        case ShaderDataType::Float4:        return GL_FLOAT;
        case ShaderDataType::Mat3:          return GL_FLOAT;
        case ShaderDataType::Mat4:          return GL_FLOAT;
        case ShaderDataType::Int:           return GL_INT;
        case ShaderDataType::Int2:          return GL_INT;
        case ShaderDataType::Int3:          return GL_INT;
        case ShaderDataType::Int4:          return GL_INT;
        case ShaderDataType::Bool:          return GL_BOOL;
    }

    CORE_ASSERT(false, "Unknown ShaderDataType");
    return 0;
}

VertexArray::VertexArray() 
{
    glCreateVertexArrays(1, &m_RendererID);
    glBindVertexArray(m_RendererID);
}

VertexArray::~VertexArray() 
{
    glDeleteVertexArrays(1, &m_RendererID);
}

void VertexArray::Bind() 
{
    glBindVertexArray(m_RendererID);
}

void VertexArray::UnBind() 
{
    glBindVertexArray(0);
}

void VertexArray::AddVertexBuffer(const Ref<VertexBuffer> &vertexBuffer) 
{
    CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "VertexBuffer has no layout!");

    glBindVertexArray(m_RendererID);
    vertexBuffer->Bind();

    int index = 0;
    for (const auto &element : vertexBuffer->GetLayout())
    {
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(
            index, 
            element.GetComponentCount(),
            ShaderDataTypeToOpenGLBaseType(element.Type), 
            element.Normalized ? GL_TRUE : GL_FALSE, 
            vertexBuffer->GetLayout().GetStride(), 
            (const void*)element.Offset
        );
        index++;
    }
    m_VertexBuffers.push_back(vertexBuffer);
}

void VertexArray::SetIndexBuffer(const Ref<IndexBuffer> &indexBuffer) 
{
    glBindVertexArray(m_RendererID);
    indexBuffer->Bind();
    m_IndexBuffer = indexBuffer;
}
