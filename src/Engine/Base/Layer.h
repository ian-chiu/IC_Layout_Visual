#pragma once

#include <string>
#include "Base/Core.h"
#include "Base/Timestep.h"
#include "Events/Event.h"

class Layer
{
public:
    Layer(const std::string &name = "Layer");
    virtual ~Layer();

    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnUpdate(Timestep ts) {}
    virtual void OnImGuiRender() {}
    virtual void OnEvent(Event &event) {}

    inline const std::string& GetName() const { return m_DebugName; }
private:
    std::string m_DebugName;
};