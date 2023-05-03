#include "Cortex/Graphics/Renderer.hpp"

namespace Cortex
{

    Renderer::Renderer(const RendererConfigInfo &configInfo)
    {
        Init(configInfo);
    }
    Renderer::~Renderer()
    {
    }
    b8 Renderer::Init(const RendererConfigInfo &configInfo)
    {
        m_Window = std::make_unique<Window>(configInfo.InitTitle.c_str(), configInfo.InitWidth, configInfo.InitHeight);
        m_Backend = std::make_unique<RenderBackend>(true);
        m_Device = std::make_unique<RenderDevice>(m_Backend, m_Window);
        m_Context = std::make_unique<RenderContext>(m_Device);
        return true;
    }
    b8 Renderer::Shutdown()
    {
        return true;
    }
    b8 Renderer::BeginFrame()
    {
        m_Window->Update();
        return true;
    }
    void Renderer::DrawFrame()
    {
    }
    b8 Renderer::EndFrame()
    {
        return true;
    }

}