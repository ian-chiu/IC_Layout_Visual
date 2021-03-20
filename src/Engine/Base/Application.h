#pragma once

#include <memory>
#include <functional>
#include "Window.h"
#include "Events/ApplicationEvent.h"
#include "Base/LayerStack.h"
#include "ImGui/ImGuiLayer.h"

class Application
{
public:
    Application();
    virtual ~Application();

    void Run();
    void OnEvent(Event &e);
    void PushLayer(Layer *layer);
    void PushOverlay(Layer *layer);

    inline static Application& Get() { return *s_Instance; }
    inline Window& GetWindow() { return *m_Window; }
private:
    bool OnWindowClose(WindowCloseEvent &e);
private:
    std::unique_ptr<Window> m_Window;
    ImGuiLayer *m_ImGuiLayer;
    bool m_Running = true;
    LayerStack m_LayerStack;
    float m_LastFrameTime = 0.0f;
private:
    static Application *s_Instance;
};