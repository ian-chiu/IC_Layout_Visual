#pragma once

#include <string>
#include <vector>
#include "Base/Core.h"

enum class ShaderDataType
{
    None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
};

static unsigned int ShaderDataTypeSize(ShaderDataType type)
{
    switch (type) 
    {
        case ShaderDataType::Float:         return 4;
        case ShaderDataType::Float2:        return 4 * 2;
        case ShaderDataType::Float3:        return 4 * 3;
        case ShaderDataType::Float4:        return 4 * 4;
        case ShaderDataType::Mat3:          return 4 * 3 * 3;
        case ShaderDataType::Mat4:          return 4 * 4 * 4;
        case ShaderDataType::Int:           return 4;
        case ShaderDataType::Int2:          return 4 * 2;
        case ShaderDataType::Int3:          return 4 * 3;
        case ShaderDataType::Int4:          return 4 * 4;
        case ShaderDataType::Bool:          return 1;
    }

    CORE_ASSERT(false, "Unknown ShaderDataType");
    return 0;
}

struct BufferElement
{
    std::string Name;
    ShaderDataType Type;
    unsigned int Offset;
    unsigned int Size;
    bool Normalized;

    BufferElement() = default;

    BufferElement(ShaderDataType type, const std::string &name, bool normalized = false) 
        : Name(name), Type(type), Offset(0), Size(ShaderDataTypeSize(type)), Normalized(normalized)
    {
    }

    unsigned int GetComponentCount() const
    {
        switch(Type)
        {
        case ShaderDataType::Float:         return 1;
        case ShaderDataType::Float2:        return 2;
        case ShaderDataType::Float3:        return 3;
        case ShaderDataType::Float4:        return 4;
        case ShaderDataType::Mat3:          return 3 * 3;
        case ShaderDataType::Mat4:          return 4 * 4;
        case ShaderDataType::Int:           return 1;
        case ShaderDataType::Int2:          return 2;
        case ShaderDataType::Int3:          return 3;
        case ShaderDataType::Int4:          return 4;
        case ShaderDataType::Bool:          return 1;
        }

        CORE_ASSERT(false, "Unknown ShaderDataType");
        return 0;
    }
};

class BufferLayout
{
public:
    BufferLayout() = default;
    BufferLayout(std::initializer_list<BufferElement> bufferElements)
        : m_Elements(bufferElements), m_Stride(0)
    {
        CalculateOffsetsAndStride();
    }

    const std::vector<BufferElement>& GetElements() const { return m_Elements; }
    unsigned int GetStride() const { return m_Stride; }
    
    std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
    std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
    std::vector<BufferElement>::const_iterator begin() const { return m_Elements.cbegin(); }
    std::vector<BufferElement>::const_iterator end() const { return m_Elements.cend(); }
private:
    void CalculateOffsetsAndStride()
    {
        m_Stride = 0;
        unsigned int offset = 0;
        for (auto &element : m_Elements)
        {
            element.Offset = offset;
            offset += element.Size;
            m_Stride += element.Size;
        }
    }
private:
    std::vector<BufferElement> m_Elements;
    unsigned int m_Stride;
};

class VertexBuffer
{
public:
    VertexBuffer(float *vertices, unsigned int size);
    ~VertexBuffer();

    void Bind() const;
    void UnBind() const;

    void SetLayout(const BufferLayout &layout) { m_Layout = layout; }
    const BufferLayout& GetLayout() const { return m_Layout; }

private:
    unsigned int m_RendererID;
    BufferLayout m_Layout;
};

class IndexBuffer
{
public:
    IndexBuffer(unsigned int *indicies, unsigned int count);
    ~IndexBuffer();

    void Bind() const;
    void UnBind() const;

    inline unsigned int GetCount() const { return m_Count; }
private:
    unsigned int m_RendererID;
    unsigned int m_Count;
};