#pragma once

#include "Cortex/Utils/Asserts.h"

#include "Cortex/Core/Window.hpp"

namespace Cortex
{
    class Application
    {
    public:
        Application();
        virtual ~Application();
        b8 Init();
        b8 Run();
        b8 Shutdown();
        void OnEvent(Event& e);
        b8 OnWindowClose(WindowCloseEvent& e);
    private:
        b8 m_Finished;
        b8 m_Suspended;
        Window* p_Window;
    };

    Application *CreateApplication();

}