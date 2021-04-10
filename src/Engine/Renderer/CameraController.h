#pragma once

#include "Renderer/Camera.h"
#include "Base/Timestep.h"

#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"

class PerspectiveCameraController
{
public:
    PerspectiveCameraController(float aspectRatio);

    void OnUpdate(Timestep ts, bool enableKeyControls = true);
    void OnEvent(Event& e);

    void OnResize(float width, float height);

    PerspectiveCamera& GetCamera() { return m_Camera; }
    const PerspectiveCamera& GetCamera() const { return m_Camera; }

    float GetTranslationSpeed() const { return m_CameraTranslationSpeed; }
    void SetTranslationSpeed(float speed);

    void SetCameraPosition(const glm::vec3 &position);

    // float GetZoomLevel() const { return m_ZoomLevel; }
    // void SetZoomLevel(float level) { m_ZoomLevel = level; }
private:
    bool OnMouseScrolled(MouseScrolledEvent& e);
    bool OnWindowResized(WindowResizeEvent& e);
    bool onMouseMoved(MouseMovedEvent &e);
    bool onKeyPressed(KeyPressedEvent &e);
private:
    float m_AspectRatio;
    // float m_ZoomLevel = 1.0f;
    PerspectiveCamera m_Camera;

    bool m_Rotation;

    glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
    // float m_CameraRotation = 0.0f; //In degrees, in the anti-clockwise direction
    float m_CameraTranslationSpeed = 10.0f, m_CameraRotationSpeed = 180.0f;
    float m_LastCamDirectionX{ 0.0f };
	float m_LastCamDirectionY{ 0.0f };
    bool m_MouseNavMode = false;
};