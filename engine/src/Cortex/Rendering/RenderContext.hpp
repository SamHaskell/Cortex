#pragma once

#include "Cortex/Rendering/VulkanUtils.hpp"

#include "Cortex/Rendering/RenderInstance.hpp"
#include "Cortex/Rendering/RenderDevice.hpp"
#include "Cortex/Rendering/Swapchain.hpp"
#include "Cortex/Rendering/Pipeline.hpp"

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

        inline const std::shared_ptr<RenderDevice>& GetDevice() { return m_Device; }
        inline const std::unique_ptr<Swapchain>& GetSwapchain() { return m_Swapchain; }

        inline void SetClearColor(VkClearColorValue color) { return m_Swapchain->SetClearColor(color); }

        RenderContext(const RenderContext &) = delete;
        RenderContext &operator=(const RenderContext &) = delete;

    private:
        std::unique_ptr<RenderInstance> m_Instance;
        std::shared_ptr<RenderDevice> m_Device;
        std::unique_ptr<Swapchain> m_Swapchain;

        std::shared_ptr<Pipeline> m_TestPipeline;
    };
}