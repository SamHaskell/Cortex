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

    // Contains Sync objects for a single frame, we will have 1 for each frame-in-flight

    struct SyncObjects
    {
        VkSemaphore ImageAvailableSemaphore;
        VkSemaphore RenderFinishedSemaphore;
        VkFence InFlightFence;
    };

    class RenderContext
    {
    public:
        RenderContext(const RenderContextConfig &contextConfig, const std::unique_ptr<Window> &window);
        ~RenderContext();

    private:
        const u32 MAX_FRAMES_IN_FLIGHT = 2;
        u32 m_CurrentFrameIndex;
        u32 m_CurrentSwapchainImageIndex;

        std::unique_ptr<RenderInstance> m_Instance;
        std::unique_ptr<RenderDevice> m_Device;
        std::unique_ptr<Swapchain> m_Swapchain;

        VkCommandPool m_MainCommandPool;
        std::vector<VkCommandBuffer> m_MainCommandBuffers;
        std::vector<SyncObjects> m_MainSyncObjects;

        VkCommandPool CreateCommandPool();
        std::vector<VkCommandBuffer> CreateCommandBuffers(u32 count);
        std::vector<SyncObjects> CreateSyncObjects(u32 count);

        void DestroySyncObjects();
    };
}