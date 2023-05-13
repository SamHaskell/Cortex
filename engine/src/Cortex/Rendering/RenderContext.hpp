#pragma once

#include "Cortex/Rendering/VulkanUtils.h"

#include "Cortex/Rendering/RenderInstance.hpp"
#include "Cortex/Rendering/RenderDevice.hpp"
#include "Cortex/Rendering/Swapchain.hpp"

#include <memory>

namespace Cortex
{
    struct RenderContextConfig
    {
        RenderInstanceConfig InstanceConfig;
        RenderDeviceConfig DeviceConfig;
        SwapchainConfig SwapchainConfig;
        static RenderContextConfig Default(u32 width, u32 height);
    };

    class RenderContext
    {
    public:
        RenderContext(const RenderContextConfig &contextConfig, const std::unique_ptr<Window> &window);
        ~RenderContext();

        void BeginFrame();
        void RecordTestCommandBuffer();
        void EndFrame();

        RenderContext(const RenderContext &) = delete;
        RenderContext &operator=(const RenderContext &) = delete;

    private:
        std::unique_ptr<RenderInstance> m_Instance;
        std::unique_ptr<RenderDevice> m_Device;
        std::unique_ptr<Swapchain> m_Swapchain;
    };
}