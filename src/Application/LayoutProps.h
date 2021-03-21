#pragma once

#include <glm/glm.hpp>
#include <string>

struct Boundary
{
    Boundary() = default;
    Boundary(glm::vec2 bl, glm::vec2 tr)
        : BottomLeft(bl), TopRight(tr), Width(tr.x - bl.x), Height(tr.y - bl.y)
    {
    }
    void reset(glm::vec2 bl, glm::vec2 tr)
    {
        BottomLeft = bl;
        TopRight = tr;
        Width = tr.x - bl.x;
        Height = tr.y - bl.y;
    }
    glm::vec2 BottomLeft;
    glm::vec2 TopRight;
    float Width;
    float Height;
};

struct Polygon
{
    Boundary Boundary;
    int NetId;
    int LayerId;
    std::string PolyType;
};