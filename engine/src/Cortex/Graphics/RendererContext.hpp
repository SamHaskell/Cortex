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

    struct SyncObjects
    {
        VkSemaphore ImageAvailableSemaphore;
        VkSemaphore RenderFinishedSemaphore;
        VkFence InFlightFence;
    };

    class RendererContext
    {
    public:
        RendererContext(const RendererContextConfig& config, const Window& window);
        ~RendererContext();
        b8 BeginFrame();
        b8 EndFrame();

        RendererContext(const RendererContext &) = delete;
        RendererContext &operator=(const RendererContext &) = delete;

    private:
        const u32 MAX_FRAMES_IN_FLIGHT = 2;
        u32 m_CurrentFrameIndex;
        u32 m_CurrentSwapchainImageIndex;

        RendererInstance m_Instance;
        RendererDevice m_Device;
        RendererSwapchain m_Swapchain;
        VkCommandPool m_CommandPool;
        std::vector<VkCommandBuffer> m_CommandBuffers;
        std::vector<SyncObjects> m_SyncObjects;

        VkCommandPool CreateCommandPool();
        std::vector<VkCommandBuffer> CreateCommandBuffers(u32 count);
        std::vector<SyncObjects> CreateSyncObjects(u32 count);

        void DestroySyncObjects();
    };
}