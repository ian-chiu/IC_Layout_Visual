#include "Renderer/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

PerspectiveCamera::PerspectiveCamera(float aspect_ratio, float fov, float near, float far)
:   m_ProjectionMatrix(glm::perspective(fov, aspect_ratio, near, far)),
    m_FOV(fov),
    m_AspectRatio(aspect_ratio),
    m_Near(near),
    m_Far(far)
{
    m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void PerspectiveCamera::RecalculateViewMatrix() 
{
    glm::vec3 direction;
    direction.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    direction.y = sin(glm::radians(m_Pitch));
    direction.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_Front = glm::normalize(direction);

    m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void PerspectiveCamera::RecalculateProjectionMatrix()
{
    m_ProjectionMatrix = glm::perspective(m_FOV, m_AspectRatio, m_Near, m_Far);
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}


void PerspectiveCamera::SetPosition(const glm::vec3 &position)
{
    m_Position = position; 
    RecalculateViewMatrix();
}

void PerspectiveCamera::SetAspectRatio(float aspectRatio)
{
    m_AspectRatio = aspectRatio; 
    RecalculateViewMatrix();
}

void PerspectiveCamera::setFar(float far) 
{
    m_Far = far;
    RecalculateProjectionMatrix();
}

void PerspectiveCamera::AddFOV(float offset) 
{
    m_FOV += offset;
    if (m_FOV < 1.0f)
        m_FOV = 1.0f;
    if (m_FOV > 45.0f)
        m_FOV = 45.0f; 
    RecalculateProjectionMatrix();
}

void PerspectiveCamera::SetFOV(float fov) 
{
    m_FOV = fov;
    if (m_FOV < 1.0f)
        m_FOV = 1.0f;
    if (m_FOV > 45.0f)
        m_FOV = 45.0f; 
    RecalculateProjectionMatrix();
}

void PerspectiveCamera::AddYaw(float offset)
{
    m_Yaw += offset; 
    RecalculateViewMatrix();
}

void PerspectiveCamera::SetYaw(float yaw)
{
    m_Yaw = yaw; 
    RecalculateViewMatrix();
}

void PerspectiveCamera::AddPitch(float offset) 
{
    m_Pitch += offset; 
    if(m_Pitch > 89.0f)
        m_Pitch =  89.0f;
    if(m_Pitch < -89.0f)
        m_Pitch = -89.0f;
    RecalculateViewMatrix();
}

void PerspectiveCamera::SetPitch(float pitch) 
{
    m_Pitch = pitch; 
    if(m_Pitch > 89.0f)
        m_Pitch =  89.0f;
    if(m_Pitch < -89.0f)
        m_Pitch = -89.0f;
    RecalculateViewMatrix();
}