#pragma once

#include "Renderer/Buffer.h"
#include "Base/Core.h"

class VertexArray
{
public:
    VertexArray();
    ~VertexArray();

    void Bind();
    void UnBind();

    void AddVertexBuffer(const Ref<VertexBuffer> &vertexBuffer);
    void SetIndexBuffer(const Ref<IndexBuffer> &indexBuffer);

    const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }
    const Ref<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }
private:
    unsigned int m_RendererID;
    std::vector<Ref<VertexBuffer>> m_VertexBuffers;
    Ref<IndexBuffer> m_IndexBuffer;
};