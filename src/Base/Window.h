#pragma once

#include "Events/Event.h"
#include <GLFW/glfw3.h>

struct WindowProps
{
    std::string Title;
    unsigned int Width;
    unsigned int Height;

    WindowProps(const std::string& title = "learn opengl",
                unsigned int width = 1280,
                unsigned int height = 720)
        : Title(title), Width(width), Height(height)
    {
    }
};

// Interface representing a desktop system based Window
class Window
{
public:
    using EventCallbackFn = std::function<void(Event&)>;
public:
    Window(const WindowProps& props = WindowProps{});
    ~Window();

    void OnUpdate();

    inline unsigned int GetWidth() const { return m_Data.Width; }
    inline unsigned int GetHeight() const { return m_Data.Height; }

    // Window attributes
    inline void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }
    void SetVSync(bool enabled);
    bool IsVSync() const;

    inline void* GetNativeWindow() const { return m_Window; }
private:
    void Init(const WindowProps& props);
    void Shutdown();
private:
    GLFWwindow *m_Window;
    // GraphicsContext *m_Context;

    struct WindowData
    {
        std::string Title;
        unsigned int Width, Height;
        bool VSync;

        EventCallbackFn EventCallback;
    };

    WindowData m_Data;
};