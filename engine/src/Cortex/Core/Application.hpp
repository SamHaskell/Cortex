#pragma once

#include "Cortex/Utils/Asserts.h"

#include "Cortex/Core/Window.hpp"
#include "Cortex/Graphics/RendererContext.hpp"

namespace Cortex
{
    class Application
    {
    public:
        Application();
        virtual ~Application();
        b8 Run();
        void OnEvent(Event &e);
        b8 OnWindowClose(WindowCloseEvent &e);
        b8 OnWindowFramebufferResize(WindowFramebufferResizeEvent &e);

    private:
        b8 m_Finished;
        b8 m_Suspended;
        std::unique_ptr<Window> m_Window;
        std::unique_ptr<RendererContext> m_Context;
    };

    Application *CreateApplication();

}