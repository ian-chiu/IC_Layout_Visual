#pragma once

#include <glm/glm.hpp>
#include <string>

struct Boundary
{
    Boundary() = default;
    Boundary(glm::vec2 bl, glm::vec2 tr)
        : BottomLeft(bl), TopRight(tr), Width(tr.x - bl.x), Height(tr.y - bl.y), Max(glm::max(Width, Height))
    {
    }
    void reset(glm::vec2 bl, glm::vec2 tr)
    {
        BottomLeft = bl;
        TopRight = tr;
        Width = tr.x - bl.x;
        Height = tr.y - bl.y;
        Max = glm::max(Width, Height);
    }
    glm::vec2 BottomLeft;
    glm::vec2 TopRight;
    float Width;
    float Height;
    float Max;
};

struct Polygon
{
    Boundary Boundary;
    int NetId;
    int LayerId;
    std::string PolyType;
};