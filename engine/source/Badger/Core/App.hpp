#pragma once

#include "Badger/Base/Base.hpp"
#include "Badger/Core/Window.hpp"
#include "Badger/Core/Events.hpp"

#include "Badger/Graphics/GraphicsContext.hpp"
#include "Badger/Graphics/Renderer.hpp"
#include "Badger/Graphics/Model.hpp"

#include "Badger/Core/Scene.hpp"
#include "Badger/Core/Camera.hpp"
#include "Badger/Entities/Entity.hpp"
#include "Badger/Entities/Transform.hpp"
#include "Badger/Entities/MeshInstance.hpp"

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
        f32 m_AspectRatio;
    };

    App *CreateApp();
}