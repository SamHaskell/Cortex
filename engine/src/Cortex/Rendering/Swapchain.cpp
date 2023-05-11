#include "Cortex/Rendering/Swapchain.hpp"

namespace Cortex
{
    SwapchainConfig SwapchainConfig::Default(u32 width, u32 height)
    {
        SwapchainConfig config;
        config.FramebufferWidth = width;
        config.FramebufferHeight = height;
        return config;
    }

    Swapchain::Swapchain(const SwapchainConfig &swapchainConfig, const std::unique_ptr<RenderInstance> &instance, const std::unique_ptr<RenderDevice> &device)
        : m_DeviceHandle(device->GetDevice())
    {
        CreateSwapchain(swapchainConfig, instance->GetSurface(), device->GetQueueFamilies(), device->GetSwapchainSupportDetails());
        CreateRenderPass();

        RetrieveImages();
        CreateImageViews();
        CreateFramebuffers();

        CreateDrawCommandPool(device->GetQueueFamilies());
        CreateDrawCommandBuffers();
        CreateDrawSyncObjects();

        LogSwapchainDetails();
    }

    Swapchain::~Swapchain()
    {
        for (auto obj : m_DrawSyncObjects)
        {
            vkDestroySemaphore(m_DeviceHandle, obj.ImageAvailableSemaphore, nullptr);
            vkDestroySemaphore(m_DeviceHandle, obj.RenderFinishedSemaphore, nullptr);
            vkDestroyFence(m_DeviceHandle, obj.InFlightFence, nullptr);
        }

        vkFreeCommandBuffers(m_DeviceHandle, m_DrawCommandPool, static_cast<u32>(m_DrawCommandBuffers.size()), m_DrawCommandBuffers.data());
        vkDestroyCommandPool(m_DeviceHandle, m_DrawCommandPool, nullptr);
        vkDestroyRenderPass(m_DeviceHandle, m_RenderPass, nullptr);

        DestroySwapchainObjects();
    }

    void Swapchain::DestroySwapchainObjects()
    {
        for (auto framebuffer : m_Framebuffers)
        {
            vkDestroyFramebuffer(m_DeviceHandle, framebuffer, nullptr);
        }
        for (auto view : m_ImageViews)
        {
            vkDestroyImageView(m_DeviceHandle, view, nullptr);
        }
        vkDestroySwapchainKHR(m_DeviceHandle, m_Swapchain, nullptr);
    }

    void Swapchain::BeginFrame()
    {
        vkWaitForFences(m_DeviceHandle, 1, &m_DrawSyncObjects[m_CurrentFrameIndex].InFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(m_DeviceHandle, 1, &m_DrawSyncObjects[m_CurrentFrameIndex].InFlightFence);

        vkAcquireNextImageKHR(m_DeviceHandle, m_Swapchain, UINT64_MAX, m_DrawSyncObjects[m_CurrentFrameIndex].ImageAvailableSemaphore, VK_NULL_HANDLE, &m_CurrentImageIndex);

        VkCommandBufferBeginInfo beginInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
        beginInfo.pNext = nullptr;

        vkBeginCommandBuffer(GetCurrentCommandBuffer(), &beginInfo);

        VkClearValue clearValue;
        clearValue.color = {{0.2f, 0.2f, 0.5f, 1.0f}};

        VkRenderPassBeginInfo passInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
        passInfo.renderPass = m_RenderPass;
        passInfo.framebuffer = m_Framebuffers[m_CurrentImageIndex];
        passInfo.renderArea.offset = {0, 0};
        passInfo.renderArea.extent = m_SwapchainExtent;
        passInfo.clearValueCount = 1;
        passInfo.pClearValues = &clearValue;

        vkCmdBeginRenderPass(m_DrawCommandBuffers[m_CurrentFrameIndex], &passInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void Swapchain::EndFrame(const VkQueue graphicsQueue)
    {
        vkCmdEndRenderPass(m_DrawCommandBuffers[m_CurrentFrameIndex]);
        VkResult result = vkEndCommandBuffer(m_DrawCommandBuffers[m_CurrentFrameIndex]);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to finish recording a Vulkan Command Buffer");

        VkSemaphore waitSemaphores[] = {m_DrawSyncObjects[m_CurrentFrameIndex].ImageAvailableSemaphore};
        VkSemaphore signalSemaphores[] = {m_DrawSyncObjects[m_CurrentFrameIndex].RenderFinishedSemaphore};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_DrawCommandBuffers[m_CurrentFrameIndex];

        result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, m_DrawSyncObjects[m_CurrentFrameIndex].InFlightFence);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to submit a Command Buffer to the Graphics Queue");
    }

    void Swapchain::PresentFrame(const VkQueue presentQueue)
    {
        VkSemaphore signalSemaphores[] = {m_DrawSyncObjects[m_CurrentFrameIndex].RenderFinishedSemaphore};
        VkSwapchainKHR swapChains[] = {m_Swapchain};

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &m_CurrentImageIndex;

        vkQueuePresentKHR(presentQueue, &presentInfo);

        m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_ImageCount;
    }

    void Swapchain::CreateSwapchain(const SwapchainConfig &config, const VkSurfaceKHR &surface, const DeviceQueueFamilies &queueDetails, const DeviceSwapchainSupportDetails &swapchainDetails)
    {
        VkSurfaceFormatKHR surfaceFormat = ChooseSwapchainSurfaceFormat(swapchainDetails.SurfaceFormats);
        VkPresentModeKHR presentMode = ChooseSwapchainPresentMode(swapchainDetails.PresentModes);
        VkExtent2D extent = ChooseSwapchainExtents(swapchainDetails.Capabilities, config.FramebufferWidth, config.FramebufferHeight);

        u32 imageCount = swapchainDetails.Capabilities.minImageCount + 1;
        if (swapchainDetails.Capabilities.maxImageCount > 0 && imageCount > swapchainDetails.Capabilities.maxImageCount)
        {
            imageCount = swapchainDetails.Capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        u32 queueIndices[2] = {queueDetails.GraphicsFamily, queueDetails.PresentFamily};
        if (queueDetails.GraphicsFamily != queueDetails.PresentFamily)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = swapchainDetails.Capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        VkResult result = vkCreateSwapchainKHR(m_DeviceHandle, &createInfo, nullptr, &m_Swapchain);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a Vulkan Swapchain");

        m_SwapchainExtent = extent;
        m_SwapchainFormat = surfaceFormat.format;
        m_SwapchainPresentMode = presentMode;
    }

    void Swapchain::CreateRenderPass()
    {
        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = m_SwapchainFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorRef = {};
        colorRef.attachment = 0;
        colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo createInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = &colorAttachment;
        createInfo.subpassCount = 1;
        createInfo.pSubpasses = &subpass;
        createInfo.dependencyCount = 1;
        createInfo.pDependencies = &dependency;

        VkResult result = vkCreateRenderPass(m_DeviceHandle, &createInfo, nullptr, &m_RenderPass);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a Vulkan RenderPass");
    }

    void Swapchain::RetrieveImages()
    {
        vkGetSwapchainImagesKHR(m_DeviceHandle, m_Swapchain, &m_ImageCount, nullptr);
        m_Images.resize(m_ImageCount);
        vkGetSwapchainImagesKHR(m_DeviceHandle, m_Swapchain, &m_ImageCount, m_Images.data());
    }

    void Swapchain::CreateImageViews()
    {
        m_ImageViews.resize(m_Images.size());
        for (size_t i = 0; i < m_ImageViews.size(); i++)
        {
            VkImageViewCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = m_Images[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = m_SwapchainFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            VkResult result = vkCreateImageView(m_DeviceHandle, &createInfo, nullptr, &m_ImageViews[i]);
            CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a Vulkan ImageView");
        }
    }

    void Swapchain::CreateFramebuffers()
    {
        m_Framebuffers.resize(m_ImageCount);

        for (size_t i = 0; i < m_Framebuffers.size(); i++)
        {
            VkImageView attachments[] = {
                m_ImageViews[i]};
            VkFramebufferCreateInfo createInfo = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
            createInfo.renderPass = m_RenderPass;
            createInfo.attachmentCount = 1;
            createInfo.pAttachments = attachments;
            createInfo.width = m_SwapchainExtent.width;
            createInfo.height = m_SwapchainExtent.height;
            createInfo.layers = 1;

            VkResult result = vkCreateFramebuffer(m_DeviceHandle, &createInfo, nullptr, &m_Framebuffers[i]);
            CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a Vulkan Framebuffer");
        }
    }

    void Swapchain::CreateDrawCommandPool(const DeviceQueueFamilies &queueDetails)
    {
        VkCommandPoolCreateInfo createInfo = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
        createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        createInfo.queueFamilyIndex = queueDetails.GraphicsFamily;

        VkResult result = vkCreateCommandPool(m_DeviceHandle, &createInfo, nullptr, &m_DrawCommandPool);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a Vulkan Command Pool");
    }

    void Swapchain::CreateDrawCommandBuffers()
    {
        m_DrawCommandBuffers.resize(m_ImageCount);

        VkCommandBufferAllocateInfo allocInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
        allocInfo.commandBufferCount = static_cast<u32>(m_DrawCommandBuffers.size());
        allocInfo.commandPool = m_DrawCommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

        VkResult result = vkAllocateCommandBuffers(m_DeviceHandle, &allocInfo, m_DrawCommandBuffers.data());
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to allocate Vulkan Command Buffers");
    }

    void Swapchain::CreateDrawSyncObjects()
    {
        m_DrawSyncObjects.resize(m_ImageCount);
        VkSemaphoreCreateInfo semaphoreInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
        VkFenceCreateInfo fenceInfo = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        for (size_t i = 0; i < m_DrawSyncObjects.size(); i++)
        {
            VkResult result_a = vkCreateSemaphore(m_DeviceHandle, &semaphoreInfo, nullptr, &m_DrawSyncObjects[i].ImageAvailableSemaphore);
            VkResult result_b = vkCreateSemaphore(m_DeviceHandle, &semaphoreInfo, nullptr, &m_DrawSyncObjects[i].RenderFinishedSemaphore);
            VkResult result_c = vkCreateFence(m_DeviceHandle, &fenceInfo, nullptr, &m_DrawSyncObjects[i].InFlightFence);
            CX_ASSERT_MSG(result_a == VK_SUCCESS && result_b == VK_SUCCESS && result_c == VK_SUCCESS, "Failed to create Vulkan Semaphore/Fence");
        }
    }

    VkSurfaceFormatKHR Swapchain::ChooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR> surfaceFormats)
    {
        for (const auto &surfaceFormat : surfaceFormats)
        {
            if (surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                if (surfaceFormat.format == VK_FORMAT_R8G8B8A8_SRGB || surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB)
                {
                    return surfaceFormat;
                }
                else if (surfaceFormat.format == VK_FORMAT_R8G8B8A8_UNORM || surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
                {
                    return surfaceFormat;
                }
            }
        }
        CX_WARN("No preferred format available, defaulting to first found");
        return surfaceFormats[0];
    }

    VkPresentModeKHR Swapchain::ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR> presentModes)
    {
        for (const auto &presentMode : presentModes)
        {
            if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return presentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D Swapchain::ChooseSwapchainExtents(const VkSurfaceCapabilitiesKHR capabilities, u32 windowWidth, u32 windowHeight)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<u32>::max())
        {
            return capabilities.currentExtent;
        }
        else
        {
            VkExtent2D trueExtent = {windowWidth, windowHeight};
            trueExtent.width = std::clamp(trueExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            trueExtent.width = std::clamp(trueExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return trueExtent;
        }
    }

    void Swapchain::LogSwapchainDetails()
    {
        CX_INFO("(Vulkan Swapchain) IMAGES: %u, PRESENT MODE: %s, FORMAT: %s", m_ImageCount, string_VkPresentModeKHR(m_SwapchainPresentMode), string_VkFormat(m_SwapchainFormat));
    }
}