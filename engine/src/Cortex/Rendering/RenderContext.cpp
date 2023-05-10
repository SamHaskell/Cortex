#include "Cortex/Rendering/RenderContext.hpp"

namespace Cortex
{
    RenderContextConfig RenderContextConfig::Default(u32 width, u32 height)
    {
        RenderContextConfig config;
        config.InstanceConfig = RenderInstanceConfig::Default();
        config.DeviceConfig = RenderDeviceConfig::Default();
        config.SwapchainConfig = SwapchainConfig::Default(width, height);
        return config;
    }

    RenderContext::RenderContext(const RenderContextConfig &contextConfig, const std::unique_ptr<Window> &window)
        : m_CurrentFrameIndex(0),
          m_Instance(std::make_unique<RenderInstance>(contextConfig.InstanceConfig, window)),
          m_Device(std::make_unique<RenderDevice>(contextConfig.DeviceConfig, m_Instance)),
          m_Swapchain(std::make_unique<Swapchain>(contextConfig.SwapchainConfig, m_Instance, m_Device)),
          m_MainCommandPool(CreateCommandPool()),
          m_MainCommandBuffers(CreateCommandBuffers(MAX_FRAMES_IN_FLIGHT)),
          m_MainSyncObjects(CreateSyncObjects(MAX_FRAMES_IN_FLIGHT))
    {
    }

    RenderContext::~RenderContext()
    {
        DestroySyncObjects();
        vkFreeCommandBuffers(m_Device->GetDevice(), m_MainCommandPool, static_cast<u32>(m_MainCommandBuffers.size()), m_MainCommandBuffers.data());
        vkDestroyCommandPool(m_Device->GetDevice(), m_MainCommandPool, nullptr);
    }

    VkCommandPool RenderContext::CreateCommandPool()
    {
        VkCommandPool pool;

        VkCommandPoolCreateInfo createInfo = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
        createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        createInfo.queueFamilyIndex = m_Device->GetQueueFamilies().GraphicsFamily;

        VkResult result = vkCreateCommandPool(m_Device->GetDevice(), &createInfo, nullptr, &pool);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a Vulkan Command Pool");

        return pool;
    }

    std::vector<VkCommandBuffer> RenderContext::CreateCommandBuffers(u32 count)
    {
        std::vector<VkCommandBuffer> buffers(count);
        VkCommandBufferAllocateInfo allocInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
        allocInfo.commandBufferCount = count;
        allocInfo.commandPool = m_MainCommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

        VkResult result = vkAllocateCommandBuffers(m_Device->GetDevice(), &allocInfo, buffers.data());
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to allocate Vulkan Command Buffers");

        return buffers;
    }

    std::vector<SyncObjects> RenderContext::CreateSyncObjects(u32 count)
    {
        std::vector<SyncObjects> syncObjects(count);
        VkSemaphoreCreateInfo semaphoreInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
        VkFenceCreateInfo fenceInfo = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        for (u32 i = 0; i < count; i++)
        {
            VkResult result_a = vkCreateSemaphore(m_Device->GetDevice(), &semaphoreInfo, nullptr, &syncObjects[i].ImageAvailableSemaphore);
            VkResult result_b = vkCreateSemaphore(m_Device->GetDevice(), &semaphoreInfo, nullptr, &syncObjects[i].RenderFinishedSemaphore);
            VkResult result_c = vkCreateFence(m_Device->GetDevice(), &fenceInfo, nullptr, &syncObjects[i].InFlightFence);
            CX_ASSERT_MSG(result_a == VK_SUCCESS && result_b == VK_SUCCESS && result_c == VK_SUCCESS, "Failed to create Vulkan Semaphore/Fence");
        }
        return syncObjects;
    }

    void RenderContext::DestroySyncObjects()
    {
        for (auto &syncObjects : m_MainSyncObjects)
        {
            vkDestroyFence(m_Device->GetDevice(), syncObjects.InFlightFence, nullptr);
            vkDestroySemaphore(m_Device->GetDevice(), syncObjects.ImageAvailableSemaphore, nullptr);
            vkDestroySemaphore(m_Device->GetDevice(), syncObjects.RenderFinishedSemaphore, nullptr);
        }
    }
}