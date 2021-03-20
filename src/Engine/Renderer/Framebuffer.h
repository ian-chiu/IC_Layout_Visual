#pragma once

#include "Base/Core.h"

struct FramebufferSpecification
{
    uint32_t Width, Height;
    // FramebufferFormat Format = 
    uint32_t Samples = 1;

    bool SwapChainTarget = false;
};

class Framebuffer
{
public:
    Framebuffer(const FramebufferSpecification& spec);
    ~Framebuffer();

    void Resize(uint32_t width, uint32_t height);
    void Bind();
    void Unbind();
    void Invalidate();
    uint32_t GetColorAttachmentRendererID() const { return m_ColorAttachment; }
    const FramebufferSpecification& GetSpecification() const;
    static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
private:
    uint32_t m_RendererID;
    uint32_t m_ColorAttachment, m_DepthAttachment;
    FramebufferSpecification m_Specification;
};