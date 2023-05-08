#include "Cortex/Graphics/RendererContext.hpp"

#include "vulkan/vulkan.h"

namespace Cortex
{
    RendererContextConfig RendererContextConfig::Default(u32 width, u32 height)
    {
        RendererContextConfig config;
        config.InstanceConfig = RendererInstanceConfig::Default();
        config.DeviceConfig = RendererDeviceConfig::Default();
        config.SwapchainConfig = RendererSwapchainConfig::Default(width, height);
        return config;
    }

    RendererContext::RendererContext(const RendererContextConfig &config, const Window &window)
        : m_CurrentFrameIndex(0),
          m_Instance(config.InstanceConfig, window),
          m_Device(config.DeviceConfig, m_Instance.GetInstance(), m_Instance.GetSurface()),
          m_Swapchain({window.GetFramebufferWidth(), window.GetFramebufferHeight()}, m_Device.GetDevice(), m_Instance.GetSurface(), m_Device.GetQueueFamilies(), m_Device.GetSwapchainSupportDetails()),
          m_CommandPool(CreateCommandPool()),
          m_CommandBuffers(CreateCommandBuffers(MAX_FRAMES_IN_FLIGHT)),
          m_SyncObjects(CreateSyncObjects(MAX_FRAMES_IN_FLIGHT))
    {
    }

    RendererContext::~RendererContext()
    {
        DestroySyncObjects();
        vkFreeCommandBuffers(m_Device.GetDevice(), m_CommandPool, static_cast<u32>(m_CommandBuffers.size()), m_CommandBuffers.data());
        vkDestroyCommandPool(m_Device.GetDevice(), m_CommandPool, nullptr);
    }

    b8 RendererContext::BeginFrame()
    {
        vkWaitForFences(m_Device.GetDevice(), 1, &m_SyncObjects[m_CurrentFrameIndex].InFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(m_Device.GetDevice(), 1, &m_SyncObjects[m_CurrentFrameIndex].InFlightFence);

        vkAcquireNextImageKHR(m_Device.GetDevice(), m_Swapchain.GetSwapchain(), UINT64_MAX, m_SyncObjects[m_CurrentFrameIndex].ImageAvailableSemaphore, VK_NULL_HANDLE, &m_CurrentSwapchainImageIndex);
        
        vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrameIndex], 0);

        return true;
    }

    b8 RendererContext::EndFrame()
    {
        VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
        VkSemaphore signalSemaphores[] = {m_SyncObjects[m_CurrentFrameIndex].RenderFinishedSemaphore};
        VkSemaphore waitSemaphores[] = {m_SyncObjects[m_CurrentFrameIndex].ImageAvailableSemaphore};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_CommandBuffers[m_CurrentFrameIndex];

        VkResult result = vkQueueSubmit(m_Device.GetGraphicsQueue(), 1, &submitInfo, m_SyncObjects[m_CurrentFrameIndex].InFlightFence);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to submit a Vulkan Command Buffer to the Graphics Queue");

        m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
        return true;
    }

    VkCommandPool RendererContext::CreateCommandPool()
    {
        VkCommandPool pool;

        VkCommandPoolCreateInfo createInfo = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
        createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        createInfo.queueFamilyIndex = m_Device.GetQueueFamilies().GraphicsFamily;

        VkResult result = vkCreateCommandPool(m_Device.GetDevice(), &createInfo, nullptr, &pool);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a Vulkan Command Pool");

        return pool;
    }

    std::vector<VkCommandBuffer> RendererContext::CreateCommandBuffers(u32 count)
    {
        std::vector<VkCommandBuffer> buffers(count);
        VkCommandBufferAllocateInfo allocInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
        allocInfo.commandBufferCount = count;
        allocInfo.commandPool = m_CommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

        VkResult result = vkAllocateCommandBuffers(m_Device.GetDevice(), &allocInfo, buffers.data());
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to allocate Vulkan Command Buffers");

        return buffers;
    }

    std::vector<SyncObjects> RendererContext::CreateSyncObjects(u32 count)
    {
        std::vector<SyncObjects> syncObjects(count);
        VkSemaphoreCreateInfo semaphoreInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
        VkFenceCreateInfo fenceInfo = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        for (u32 i = 0; i < count; i++) {
            VkResult result_a = vkCreateSemaphore(m_Device.GetDevice(), &semaphoreInfo, nullptr, &syncObjects[i].ImageAvailableSemaphore);
            VkResult result_b = vkCreateSemaphore(m_Device.GetDevice(), &semaphoreInfo, nullptr, &syncObjects[i].RenderFinishedSemaphore);
            VkResult result_c = vkCreateFence(m_Device.GetDevice(), &fenceInfo, nullptr, &syncObjects[i].InFlightFence);
            CX_ASSERT_MSG(result_a == VK_SUCCESS && result_b == VK_SUCCESS && result_c == VK_SUCCESS, "Failed to create Vulkan Semaphore/Fence");
        }
        return syncObjects;
    }

    void RendererContext::DestroySyncObjects()
    {
        for (auto& syncObjects : m_SyncObjects) {
            vkDestroyFence(m_Device.GetDevice(), syncObjects.InFlightFence, nullptr);
            vkDestroySemaphore(m_Device.GetDevice(), syncObjects.ImageAvailableSemaphore, nullptr);
            vkDestroySemaphore(m_Device.GetDevice(), syncObjects.RenderFinishedSemaphore, nullptr);        
        }
    }

}