#include "Base/Core.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include <glad/glad.h>

Renderer::SceneData *Renderer::m_SceneData = new Renderer::SceneData;

void Renderer::BeginScene(PerspectiveCamera &camera)
{
    m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
}

void Renderer::EndScene() 
{
}

void Renderer::Submit(
    const Ref<Shader> &shader, 
    const Ref<VertexArray> &vertexArray,
    const glm::mat4 &transform,
    GLenum mode 
) 
{
    shader->Bind();
    shader->UploadUniformMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
    shader->UploadUniformMat4("u_Transform", transform);
    vertexArray->Bind();
    glDrawElements(
        mode, 
        vertexArray->GetIndexBuffer()->GetCount(),
        GL_UNSIGNED_INT,
        nullptr
    );
}