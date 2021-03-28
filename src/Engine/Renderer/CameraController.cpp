#include "Renderer/CameraController.h"
#include "Base/Input.h"
#include "Base/KeyCodes.h"
#include "Base/Application.h"
#include <GLFW/glfw3.h>

PerspectiveCameraController::PerspectiveCameraController(float aspectRatio)
    : m_AspectRatio(aspectRatio),
      m_Camera(aspectRatio)
{
}

void PerspectiveCameraController::OnUpdate(Timestep ts)
{
    if (Input::IsKeyPressed(KEY_W)) 
        m_CameraPosition += (ts * m_CameraTranslationSpeed) * m_Camera.GetFront();
    else if (Input::IsKeyPressed(KEY_S)) 
        m_CameraPosition -= (ts * m_CameraTranslationSpeed) * m_Camera.GetFront();

    if (Input::IsKeyPressed(KEY_A)) 
        m_CameraPosition -= glm::normalize(glm::cross(m_Camera.GetFront(), m_Camera.GetUp())) * (ts * m_CameraTranslationSpeed);
    else if (Input::IsKeyPressed(KEY_D)) 
        m_CameraPosition += glm::normalize(glm::cross(m_Camera.GetFront(), m_Camera.GetUp())) * (ts * m_CameraTranslationSpeed);

    if (Input::IsKeyPressed(KEY_SPACE) || Input::IsKeyPressed(KEY_LEFT_SHIFT)) 
        m_CameraPosition += m_Camera.GetUp() * (ts * m_CameraTranslationSpeed);
    else if (Input::IsKeyPressed(KEY_LEFT_CONTROL)) 
        m_CameraPosition -= m_Camera.GetUp() * (ts * m_CameraTranslationSpeed);

    if (Input::IsKeyPressed(KEY_UP) )
        m_Camera.AddPitch(m_CameraRotationSpeed * ts);
    else if (Input::IsKeyPressed(KEY_DOWN))
        m_Camera.AddPitch(-m_CameraRotationSpeed * ts);

    if (Input::IsKeyPressed(KEY_RIGHT))
        m_Camera.AddYaw(m_CameraRotationSpeed * ts);
    else if (Input::IsKeyPressed(KEY_LEFT))
        m_Camera.AddYaw(-m_CameraRotationSpeed * ts);

    if (Input::IsKeyPressed(KEY_KP_ADD) && m_CameraTranslationSpeed <= 50.0f)
        m_CameraTranslationSpeed += 0.1f;
    else if (Input::IsKeyPressed(KEY_KP_SUBTRACT) && m_CameraTranslationSpeed >= 1.0f)
        m_CameraTranslationSpeed -= 0.1f;

    m_Camera.SetPosition(m_CameraPosition);
}

void PerspectiveCameraController::OnEvent(Event& e)
{
    EventDispatcher dispatcher(e);
    if (m_MouseNavMode)
    {
        dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(PerspectiveCameraController::onMouseMoved));
        dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(PerspectiveCameraController::OnMouseScrolled));
    }
    dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(PerspectiveCameraController::onKeyPressed));
}

void PerspectiveCameraController::OnResize(float width, float height)
{
    m_AspectRatio = width / height;
    m_Camera.SetAspectRatio(m_AspectRatio);
}

void PerspectiveCameraController::SetTranslationSpeed(float speed) 
{
    m_CameraTranslationSpeed = speed;
}

void PerspectiveCameraController::SetCameraPosition(const glm::vec3 &position) 
{
    m_CameraPosition = position;
}

bool PerspectiveCameraController::OnMouseScrolled(MouseScrolledEvent& e)
{
    float yoffset = e.GetYOffset();
    const float sensitivity = 0.1f;
    yoffset *= sensitivity;
    m_Camera.AddFOV(-yoffset);
    return true;
}

bool PerspectiveCameraController::onKeyPressed(KeyPressedEvent &e)
{
    if (e.GetKeyCode() == KEY_F1)
    {
        if (!m_MouseNavMode) {
            glfwSetInputMode((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else {
            glfwSetInputMode((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        m_MouseNavMode = !m_MouseNavMode;

        return true;
    }
    return false;
}

bool PerspectiveCameraController::onMouseMoved(MouseMovedEvent &e) 
{
    float xoffset = e.GetX() - m_LastCamDirectionX;
    float yoffset = m_LastCamDirectionY - e.GetY();

    m_LastCamDirectionX = e.GetX();
    m_LastCamDirectionY = e.GetY();

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    m_Camera.AddYaw(xoffset);
    m_Camera.AddPitch(yoffset);

    return true;
}

bool PerspectiveCameraController::OnWindowResized(WindowResizeEvent& e)
{
    OnResize((float)e.GetWidth(), (float)e.GetHeight());
    return false;
}