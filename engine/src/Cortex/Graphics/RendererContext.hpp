#pragma once

#include "vulkan/vulkan.h"
#include <vector>

#include "Cortex/Utils/Logging.hpp"
#include "Cortex/Core/Window.hpp"
#include "Cortex/Graphics/RendererInstance.hpp"
#include "Cortex/Graphics/RendererDevice.hpp"
#include "Cortex/Graphics/RendererSwapchain.hpp"

namespace Cortex
{
    struct RendererContextConfig
    {
        RendererInstanceConfig InstanceConfig;
        RendererDeviceConfig DeviceConfig;
        RendererSwapchainConfig SwapchainConfig;
        static RendererContextConfig Default(u32 width, u32 height);
    };

    struct FrameData
    {
        VkCommandPool CommandPool;
        VkCommandBuffer CommandBuffer;
        VkSemaphore ImageAvailableSemaphores;
        VkSemaphore RenderFinishedSemaphores;
        VkFence InFlightFences;
    };

    class RendererContext
    {
    public:
        RendererContext(const RendererContextConfig& config, const Window& window);
        ~RendererContext() {};
        b8 BeginFrame();
        b8 EndFrame();

        RendererContext(const RendererContext &) = delete;
        RendererContext &operator=(const RendererContext &) = delete;

    private:
        const u32 MAX_FRAMES_IN_FLIGHT = 2;
        u32 m_CurrentFrameIndex;

        RendererInstance m_Instance;
        RendererDevice m_Device;
        RendererSwapchain m_Swapchain;
        std::vector<FrameData> m_Frames;
    };
}