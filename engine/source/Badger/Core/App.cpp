#include "Badger/Core/App.hpp"

#include <iostream>

namespace Badger
{
    App::App()
        : m_Running(true)
    {
        m_Window = std::make_unique<Window>();
        m_Window->SetEventCallback(std::bind(&App::OnEvent, this, std::placeholders::_1));

        m_GraphicsContext = std::make_unique<GraphicsContext>(m_Window);
        m_Renderer = std::make_unique<Renderer>(m_GraphicsContext);

        m_AspectRatio = m_Window->GetAspectRatio();
    }

    App::~App()
    {
        
    }

    bool App::Run()
    {
        // Load a model here ...

        std::vector<VulkanVertex> cubeVertices = {
            {.Position = {-.5f, -.5f, +.5f}, .Color = {1.0f, 1.0f, 1.0f}},
            {.Position = {+.5f, -.5f, +.5f}, .Color = {1.0f, 1.0f, 1.0f}},
            {.Position = {-.5f, +.5f, +.5f}, .Color = {1.0f, 1.0f, 1.0f}},
            {.Position = {+.5f, +.5f, +.5f}, .Color = {1.0f, 1.0f, 1.0f}},
            {.Position = {-.5f, -.5f, -.5f}, .Color = {1.0f, 1.0f, 1.0f}},
            {.Position = {+.5f, -.5f, -.5f}, .Color = {1.0f, 1.0f, 1.0f}},
            {.Position = {-.5f, +.5f, -.5f}, .Color = {1.0f, 1.0f, 1.0f}},
            {.Position = {+.5f, +.5f, -.5f}, .Color = {1.0f, 1.0f, 1.0f}},
        };

        std::vector<VulkanIndex> cubeIndices = {
            //Top
            0, 4, 5,
            0, 5, 1,

            //Bottom
            2, 7, 6,
            2, 3, 7,

            //Left
            6, 4, 0,
            6, 0, 2,

            //Right
            3, 1, 5,
            3, 5, 7,

            //Front
            2, 0, 1,
            2, 1, 3,

            //Back
            7, 5, 4,
            7, 4, 6
        };

        std::vector<VulkanVertex> quadVertices = {
            {.Position = {-.5f, -.5f, .0f}, .Normal = {0.0f, 0.0f, 1.0f}, .Color = {1.0f, 1.0f, 1.0f}, .TexCoord = {1.0f, 0.0f}},
            {.Position = {+.5f, -.5f, .0f}, .Normal = {0.0f, 0.0f, 1.0f}, .Color = {1.0f, 1.0f, 1.0f}, .TexCoord = {0.0f, 0.0f}},
            {.Position = {+.5f, +.5f, .0f}, .Normal = {0.0f, 0.0f, 1.0f}, .Color = {1.0f, 1.0f, 1.0f}, .TexCoord = {0.0f, 1.0f}},
            {.Position = {-.5f, +.5f, .0f}, .Normal = {0.0f, 0.0f, 1.0f}, .Color = {1.0f, 1.0f, 1.0f}, .TexCoord = {1.0f, 1.0f}},
        };

        std::vector<VulkanIndex> quadIndices = {
            0, 1, 2, 2, 3, 0
        };

        std::shared_ptr<Model> cubeModel = m_GraphicsContext->LoadModel(cubeVertices, cubeIndices);
        std::shared_ptr<Model> quadModel = m_GraphicsContext->LoadModel(quadVertices, quadIndices);
        std::shared_ptr<Model> testModel = m_GraphicsContext->LoadModelFromOBJ("../../testbed/assets/models/viking/viking_room.obj");

        Entity cube = Entity::Create();
        cube.Mesh = {cubeModel};

        Entity quad = Entity::Create();
        quad.Mesh = {quadModel};
        
        Scene scene;

        // quad.Transform.ModelToWorld = glm::translate(glm::mat4(1.0f), {0.0f, 0.0f, -1.0f});
        // scene.Entities.push_back(quad);
        
        // for (i32 i = -40; i < 41; i++) {
        //     for (i32 j = -30; j < 31; j++) {
        //         cube.Transform.ModelToWorld = glm::translate(glm::mat4(1.0f), {2.0f * (f32)i, 2.0f * (f32)j, -80.0f});
        //         scene.Entities.push_back(cube);
        //     }
        // }

        Entity test = Entity::Create();
        test.Mesh = {testModel};
        test.Transform.ModelToWorld = glm::translate(glm::mat4(1.0f), {0.0f, 0.0f, 0.0f});
        scene.Entities.push_back(test);

        scene.MainCamera.SetView({1.2f, 1.2f, 1.2f}, {-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 1.0f});

        f64 dt = 0.0;
        f64 elapsed = 0.0;
        auto now = std::chrono::high_resolution_clock::now();
        while (m_Running) {    
            m_Window->Update(dt);

            scene.MainCamera.SetPerspectiveProjection(glm::radians(70.0f), m_AspectRatio, 0.01f, 1000.0f);

            for (auto& e : scene.Entities) {
                if (e.Mesh.Model == testModel) {
                    e.Transform.ModelToWorld = glm::rotate(e.Transform.ModelToWorld, 0.1f * (f32)dt * (f32)glm::sin(0.5*elapsed), {0.0f, 0.0f, 1.0f});
                }
            }

            VkCommandBuffer cmd;
            u32 frameIndex;
            if (m_GraphicsContext->BeginFrame(cmd)) {
                m_GraphicsContext->BeginRenderPass(cmd);
                m_Renderer->DrawScene(cmd, scene);
                m_GraphicsContext->EndRenderPass(cmd);
                m_GraphicsContext->EndFrame();
            }

            auto next = std::chrono::high_resolution_clock::now();
            dt = std::chrono::duration<f64, std::chrono::seconds::period>(next - now).count();
            elapsed += dt;
            now = next;

            LOG_DEBUG("Frame-Time: %lfms", dt*1000.0);
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
            break;
        case EventTag::WindowFramebufferSizeEvent:
            m_GraphicsContext->OnFramebufferResize(e.WindowFramebufferSizeEvent.Width, e.WindowFramebufferSizeEvent.Height);
            m_AspectRatio = m_Window->GetAspectRatio();
            break;
        default:
            break;
        }
        return true;
    }
}