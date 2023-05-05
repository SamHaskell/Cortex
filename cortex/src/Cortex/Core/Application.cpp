#include "Cortex/Core/Application.hpp"
#include "Cortex/Utils/Logging.hpp"

#include <iostream>

namespace Cortex
{
    Application::Application()
    {
        m_Finished = false;
        m_Suspended = false;

        m_Window = std::make_unique<Window>("Cortex Application", 800, 600);
        m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

        m_Renderer = std::make_unique<Renderer>(m_Window);
        m_Renderer->Init();
    }
    Application::~Application()
    {
    }
    b8 Application::Run()
    {
        while (!m_Finished)
        {
            m_Window->Update();
            m_Renderer->DrawTestFrame();
        }
        return true;
    }
    void Application::OnEvent(Event &e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));
        dispatcher.Dispatch<WindowFramebufferResizeEvent>(std::bind(&Application::OnWindowFramebufferResize, this, std::placeholders::_1));
    }
    b8 Application::OnWindowClose(WindowCloseEvent &e)
    {
        m_Finished = true;
        return true;
    }
    b8 Application::OnWindowFramebufferResize(WindowFramebufferResizeEvent &e)
    {
        m_Renderer->OnWindowFramebufferResize(e);
        return true;
    }
}