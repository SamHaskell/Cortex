#include "Cortex/Core/Application.hpp"
#include "Cortex/Utils/Logging.hpp"
#include "Cortex/Rendering/RenderContext.hpp"
#include "Cortex/Rendering/Shader.hpp"
#include "Cortex/Rendering/Pipeline.hpp"

#include <iostream>

namespace Cortex
{

    Application::Application()
    {

        m_Finished = false;
        m_Suspended = false;

        m_Window = std::make_unique<Window>("Cortex Application", 800, 600);
        m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

        m_Context = std::make_unique<RenderContext>(RenderContextConfig::Default((u32)m_Window->GetFramebufferWidth(), (u32)m_Window->GetFramebufferHeight()), m_Window);
        m_Context->SetClearColor({0.7, 0.3, 0.3, 1.0});

        std::shared_ptr<Shader> testFrag = std::make_shared<Shader>(m_Context, "../engine/assets/Shaders/basic.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
        std::shared_ptr<Shader> testVert = std::make_shared<Shader>(m_Context, "../engine/assets/Shaders/basic.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
        std::shared_ptr<Pipeline> testPipeline = std::make_shared<Pipeline>(m_Context, testVert, testFrag);
    }
    Application::~Application()
    {
    }
    b8 Application::Run()
    {
        while (!m_Finished)
        {
            m_Window->Update(); // Polls Events
            m_Context->BeginFrame();
            m_Context->RecordTestCommandBuffer();
            m_Context->EndFrame();
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
        return true;
    }
}