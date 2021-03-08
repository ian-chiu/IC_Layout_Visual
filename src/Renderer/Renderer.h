#pragma once

#include "Base/Core.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Camera.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

class Renderer
{
public:
    static void BeginScene(PerspectiveCamera &camera);
    static void EndScene();

    static void Submit(
        const Ref<Shader> &shader, 
        const Ref<VertexArray> &vertexArray,
        const glm::mat4 &transform = glm::mat4(1.0f),
        unsigned int mode = GL_TRIANGLES
    );
    
private:
    struct SceneData
    {
        glm::mat4 ViewProjectionMatrix;
    };

    static SceneData *m_SceneData;
};