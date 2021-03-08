#pragma once

#include "Base/Core.h"
#include "Renderer/VertexArray.h"
#include <glm/glm.hpp>

class RenderCommand
{
public:
    static void Init();
    static void SetClearColor(const glm::vec4 &color);
    static void Clear();
    static void DrawIndexed(const Ref<VertexArray> &vertexArray);
};