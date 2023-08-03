#pragma once

#include "Badger/Base/Base.hpp"
#include "Badger/Core/Window.hpp"
#include "Badger/Core/Events.hpp"

#include "Badger/Graphics/GraphicsContext.hpp"
#include "Badger/Graphics/Renderer.hpp"

namespace Badger
{
    class App
    {
    public:
        App();
        virtual ~App();
        bool Run();
        bool OnEvent(Event &e);

    private:
        bool m_Running;
        std::unique_ptr<Window> m_Window;
        std::unique_ptr<GraphicsContext> m_GraphicsContext;
        std::unique_ptr<Renderer> m_Renderer;
    };

    App *CreateApp();
}