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
        f64 dt = 0.0; // TEMP
        while (m_Running) {
            m_Window->Update(dt);

            m_GraphicsContext->BeginFrame();

                // Render stuff here...

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