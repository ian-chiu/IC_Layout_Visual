#pragma once

#include <glm/glm.hpp>
class PerspectiveCamera
{
public:
    PerspectiveCamera() = default;
    PerspectiveCamera(float fov, float aspect_ratio, float near = 0.1f, float far = 50.0f);

    inline const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
    inline const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
    inline const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }

    inline const glm::vec3& GetPosition() const { return m_Position; }
    inline const glm::vec3& GetFront() const { return m_Front; }
    inline const glm::vec3& GetUp() const { return m_Up; }
    inline float GetPitch() const { return m_Pitch; }
    inline float GetYaw() const { return m_Yaw; }
    inline float GetFOV() const { return m_FOV; }
    inline float GetFar() const { return m_Far; }

    void SetPosition(const glm::vec3 &position);

    void setFar(float far);

    void SetFOV(float fov);
    void AddFOV(float offset);

    void SetPitch(float pitch);
    void AddPitch(float offset);

    void SetYaw(float yaw);
    void AddYaw(float offset);
private:
    void RecalculateViewMatrix();
    void RecalculateProjectionMatrix();
private:
    glm::mat4 m_ProjectionMatrix;
    glm::mat4 m_ViewMatrix;
    glm::mat4 m_ViewProjectionMatrix;
    float m_FOV;
    float m_AspectRatio;
    float m_Near;
    float m_Far;

    glm::vec3 m_Position{ 0.0f, 0.0f, 0.0f };
    glm::vec3 m_Front{ 0.0f, 0.0f, 0.0f };
    glm::vec3 m_Up{ 0.0f, 1.0f, 0.0f };
    float m_Pitch{ 0.0f };
    float m_Yaw{ -89.0f };
};