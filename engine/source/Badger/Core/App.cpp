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
        // Load a model here ...

        std::vector<VulkanVertex> cubeVertices = {
            // left face (white)
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

            // right face (yellow)
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},

            // top face (orange, remember y axis points down)
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

            // bottom face (red)
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},

            // nose face (blue)
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

            // tail face (green)
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        };

        std::shared_ptr<Model> cubeModel = m_GraphicsContext->LoadModel(cubeVertices);

        glm::mat4 t = glm::translate(glm::mat4(1.0f), {1.0f, -1.0f, 5.0f});

        Entity cube = Entity::Create();
        cube.Mesh = {cubeModel};
        cube.Transform = {t};

        Scene scene;
        scene.Entities.push_back(cube);
        // scene.MainCamera.SetView({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f});

        f64 dt = 0.0; // TEMP
        while (m_Running) {    
            scene.MainCamera.SetPerspectiveProjection(glm::radians(60.0f), 1.8f, 0.01f, 1000.0f);
            scene.Entities[0].Transform.ModelToWorld = glm::rotate(scene.Entities[0].Transform.ModelToWorld, 0.001f, {0.2f, 0.7f, -1.0f});

            m_Window->Update(dt);

            auto cmd = m_GraphicsContext->BeginFrame();
            m_GraphicsContext->BeginRenderPass(cmd);

            m_Renderer->DrawScene(cmd, scene);

            m_GraphicsContext->EndRenderPass(cmd);
            m_GraphicsContext->EndFrame();
        }

        vkDeviceWaitIdle(m_GraphicsContext->GetDevice()->Device);

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