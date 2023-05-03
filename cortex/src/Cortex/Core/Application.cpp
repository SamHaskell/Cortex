#include "Cortex/Core/Application.hpp"
#include "Cortex/Utils/Logging.hpp"

#include <iostream>

namespace Cortex
{
    Application::Application()
    {
        CX_INFO("Engine Initialising ...");

        m_Finished = false;
        m_Suspended = false;

        RendererConfigInfo rendererConfig = {};

        p_Renderer = std::make_unique<Renderer>(rendererConfig);
        p_Renderer->SetWindowEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
    }
    Application::~Application()
    {
    }
    b8 Application::Run()
    {
        while (!m_Finished)
        {
            p_Renderer -> BeginFrame();

            p_Renderer -> EndFrame();
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
        CX_INFO("%s", e.ToString().c_str());
        return true;
    }
}