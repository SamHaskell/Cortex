#include "Cortex/Core/Application.hpp"
#include "Cortex/Utils/Logging.hpp"

#include <iostream>

namespace Cortex
{
    Application::Application()
    {
    }
    Application::~Application()
    {
    }
    b8 Application::Init()
    {
        /* TODO: THIS IS WHERE WE START UP ENGINE SUBSYSTEMS.
           With few exceptions, if we start something up here, we should
           call the corresponding Shutdown() method in the application Shutdown().
           The shutdown calls should occur in reverse order to the Init() calls. */
        CX_INFO("Engine Initialising ...");

        m_Finished = CX_FALSE;
        m_Suspended = CX_FALSE;

        p_Window = new Window();
        p_Window -> SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

        if (!p_Window->Init("Cortex Application", 1080, 720))
        {
            return CX_FALSE;
        }

        return CX_TRUE;
    }
    b8 Application::Run()
    {
        while (!m_Finished)
        {
            p_Window->Update(m_Suspended);
        }

        return CX_TRUE;
    }
    b8 Application::Shutdown()
    {

        if (!p_Window->Shutdown())
        {
            return CX_FALSE;
        }
        delete p_Window;

        return CX_TRUE;
    }
    void Application::OnEvent(Event& e){
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));
        CX_TRACE(e.ToString().c_str());
    }
    b8 Application::OnWindowClose(WindowCloseEvent& e) {
        m_Finished = CX_TRUE;
        return CX_TRUE;
    }
}