#pragma once

#include "Cortex/Utils/Asserts.h"

#include "Cortex/Graphics/RendererBackend.hpp"
#include "Cortex/Graphics/RendererContext.hpp"

#include "Cortex/Events/SystemEvent.hpp"

#include <memory>

namespace Cortex
{
    class Window;

    class Renderer
    {
    public:
        Renderer(const std::unique_ptr<Window> &window);
        ~Renderer();
        b8 Init();
        inline b8 DrawTestFrame() { return m_Context->DrawTestFrame(); }
        b8 Shutdown();

        b8 OnWindowFramebufferResize(WindowFramebufferResizeEvent &e);

    private:
        std::unique_ptr<RendererBackend> m_Backend;
        std::unique_ptr<RendererContext> m_Context;
    };
}