#include "Base/Application.h"
#include "Base/Input.h"
#include "Base/Core.h"
#include "Base/Timestep.h"
#include "Renderer/Renderer.h"

//Temporary
#include <GLFW/glfw3.h>

Application* Application::s_Instance = nullptr;

Application::Application()
{
    CORE_ASSERT(!s_Instance, "Application already exists[singleton]!")
    s_Instance = this;
    m_Window = std::make_unique<Window>();
    m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

    m_ImGuiLayer = new ImGuiLayer();
    PushOverlay(m_ImGuiLayer);
}

Application::~Application() 
{
    
}

void Application::OnEvent(Event &e) 
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));

    for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
    {
        (*--it)->OnEvent(e);
        if (e.Handled)
            break;
    }
}

void Application::PushLayer(Layer *layer) 
{
    m_LayerStack.PushLayer(layer);
    layer->OnAttach();
}

void Application::PushOverlay(Layer *layer) 
{
    m_LayerStack.PushOverlay(layer);
    layer->OnAttach();
}

void Application::Run() 
{
    while (m_Running)
    {
        // TEMPORARY: Platform::GetTime()
        float time = (float)glfwGetTime();
        Timestep timestep = time - m_LastFrameTime;
        m_LastFrameTime = time;

        for (Layer *layer : m_LayerStack)
            layer->OnUpdate(timestep);

        m_ImGuiLayer->Begin();
        for (Layer *layer : m_LayerStack)
            layer->OnImGuiRender();
        m_ImGuiLayer->End();

        m_Window->OnUpdate();
    }
}

bool Application::OnWindowClose(WindowCloseEvent &e) 
{
    m_Running = false;
    return true;
}