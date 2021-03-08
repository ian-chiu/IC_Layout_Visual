#pragma once

#include "Base/Layer.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"
#include "Events/ApplicationEvent.h"

class ImGuiLayer : public Layer
{
public:
    ImGuiLayer();
    ~ImGuiLayer();

    void OnAttach() override;
    void OnDetach() override;
    void OnImGuiRender() override;

    void Begin();
    void End();
private:
    float m_Time = 0.0f;
};