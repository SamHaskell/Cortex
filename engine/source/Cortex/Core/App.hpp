#pragma once

#include "Cortex/Base/Base.hpp"
#include "Cortex/Core/Window.hpp"
#include "Cortex/Core/Events.hpp"

#include "Cortex/Graphics/GraphicsContext.hpp"
#include "Cortex/Graphics/Renderer.hpp"
#include "Cortex/Graphics/Model.hpp"

#include "Cortex/Core/Scene.hpp"
#include "Cortex/Core/Camera.hpp"
#include "Cortex/Entities/Entity.hpp"
#include "Cortex/Entities/Transform.hpp"
#include "Cortex/Entities/MeshInstance.hpp"

namespace Cortex
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
        f32 m_AspectRatio;
    };

    App *CreateApp();
}