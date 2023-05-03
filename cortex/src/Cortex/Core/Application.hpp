#pragma once

#include "Cortex/Utils/Asserts.h"

#include "Cortex/Graphics/Renderer.hpp"

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
        std::unique_ptr<Renderer> p_Renderer;
    };

    Application *CreateApplication();

}