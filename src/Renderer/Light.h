#pragma once

#include <glm/glm.hpp>

struct Light
{
    Light
    (
        glm::vec3 a = glm::vec3{0.0f}, 
        glm::vec3 d = glm::vec3{0.0f}, 
        glm::vec3 s = glm::vec3{0.0f}
    ) : Ambient(a), Diffuse(d), Specular(s) {}
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    glm::vec3 Specular;
};

struct PointLight : public Light
{
    PointLight(
        glm::vec3 position = glm::vec3{0.0f},
        glm::vec3 ambient = glm::vec3{0.0f}, 
        glm::vec3 diffuse = glm::vec3{0.0f},
        glm::vec3 specular = glm::vec3{0.0f},
        float constant = 1.0f, float linear = 0.09f, float quadratic = 0.032f
    ) :
        Light(ambient, diffuse, specular), 
        Position(position), 
        Constant(constant), 
        Linear(linear), 
        Quadratic(quadratic) 
    {}

    glm::vec3 Position;
    float Constant;
    float Linear;
    float Quadratic;
};

struct DirLight : public Light
{
    DirLight(
        glm::vec3 direction = glm::vec3{0.0f},
        glm::vec3 ambient = glm::vec3{0.0f}, 
        glm::vec3 diffuse = glm::vec3{0.0f},
        glm::vec3 specular = glm::vec3{0.0f}
    ) : Light(ambient, diffuse, specular), Direction(direction) {}
    glm::vec3 Direction;
};