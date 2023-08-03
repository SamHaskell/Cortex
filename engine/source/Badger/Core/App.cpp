#include "Badger/Core/App.hpp"

namespace Badger
{
    App::App()
        : m_Running(true)
    {
        m_Window = std::make_unique<Window>();
        m_Window->SetEventCallback(std::bind(&App::OnEvent, this, std::placeholders::_1));

        m_GraphicsContext = std::make_unique<GraphicsContext>(m_Window);
        m_Renderer = std::make_unique<Renderer>(m_GraphicsContext);
    }

    App::~App()
    {
        
    }

    bool App::Run()
    {
        // Make some game objects here ...
        std::vector<Entity> scene;

        f64 dt = 0.0; // TEMP
        while (m_Running) {
            m_Window->Update(dt);

            auto cmd = m_GraphicsContext->BeginFrame();
            m_GraphicsContext->BeginRenderPass(cmd);

            m_Renderer->DrawEntities(scene);

            m_GraphicsContext->EndRenderPass(cmd);
            m_GraphicsContext->EndFrame();
        }
        return true;
    }

    bool App::OnEvent(Event &e)
    {
        switch (e.Tag)
        {
        case EventTag::WindowCloseEvent:
            m_Running = false;
            break;
        case EventTag::KeyEvent:
            LOG_INFO("Key Press: %i", e.KeyEvent.KeyCode);
        default:
            break;
        }
        return true;
    }
}