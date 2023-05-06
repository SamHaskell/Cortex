#include "Cortex/Graphics/Renderer.hpp"

#include "Cortex/Core/Window.hpp"

namespace Cortex
{
    Renderer::Renderer(const std::unique_ptr<Window> &window)
        : m_Backend(std::make_unique<RendererBackend>(window)),
          m_Context(std::make_unique<RendererContext>(m_Backend, window->GetFramebufferWidth(), window->GetFramebufferHeight()))
    {
    }

    Renderer::~Renderer()
    {
    }

    b8 Renderer::Init()
    {
        return true;
    }

    b8 Renderer::Shutdown()
    {
        return true;
    }

    b8 Renderer::OnWindowFramebufferResize(WindowFramebufferResizeEvent &e)
    {
        // TODO: Recreate the Context with new Framebuffer size!
        return true;
    }
}