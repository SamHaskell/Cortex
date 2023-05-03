#pragma once

#include "Cortex/Utils/Asserts.h"

#include "Cortex/Graphics/Window.hpp"
#include "Cortex/Graphics/RenderBackend.hpp"
#include "Cortex/Graphics/RenderDevice.hpp"
#include "Cortex/Graphics/RenderContext.hpp"

#include <memory>

namespace Cortex
{
    using EventCallback = std::function<void(Event &)>;

    struct RendererConfigInfo
    {
        u32 InitWidth = 800;
        u32 InitHeight = 600;
        std::string InitTitle = "Cortex Application";
    };

    class Renderer
    {
    public:
        Renderer(const RendererConfigInfo &configInfo);
        ~Renderer();
        b8 Init(const RendererConfigInfo &configInfo);
        b8 Shutdown();

        b8 BeginFrame();
        void DrawFrame();
        b8 EndFrame();

        void SetWindowEventCallback(const EventCallback &cb) { return m_Window->SetEventCallback(cb); }

    private:
        std::unique_ptr<Window> m_Window;
        std::unique_ptr<RenderBackend> m_Backend;
        std::unique_ptr<RenderDevice> m_Device;
        std::unique_ptr<RenderContext> m_Context;
    };
}