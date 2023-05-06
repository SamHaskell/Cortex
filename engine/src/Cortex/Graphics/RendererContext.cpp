#include "Cortex/Graphics/RendererContext.hpp"

#include "Cortex/Core/Window.hpp"

#include <fstream>

namespace Cortex
{
    VkVertexInputBindingDescription Vertex::GetBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    std::array<VkVertexInputAttributeDescription, 2> Vertex::GetAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);
        return attributeDescriptions;
    }

    RendererContext::RendererContext(const std::unique_ptr<RendererBackend> &backend, u32 windowWidth, u32 windowHeight)
        : m_DeviceHandle(backend->GetDevice()),
          m_GraphicsQueueHandle(backend->GetGraphicsQueue()),
          m_PresentQueueHandle(backend->GetPresentQueue()),
          m_Swapchain(CreateSwapchain(backend, windowWidth, windowHeight)),
          m_SwapchainImages(RetrieveSwapchainImages()),
          m_SwapchainImageViews(CreateSwapchainImageViews()),
          m_RenderPass(CreateRenderPass()),
          m_GraphicsPipeline(CreateGraphicsPipeline()),
          m_Framebuffers(CreateFramebuffers()),
          m_TestBuffer(CreateVertexBuffer()),
          m_CommandBuffers(CreateCommandBuffers(backend))
    {
        CreateSynchronisationObjects();
    }

    RendererContext::~RendererContext()
    {
        vkDeviceWaitIdle(m_DeviceHandle);

        for (size_t i = 0; i < m_FrameFlightFences.size(); i++)
        {
            vkDestroyFence(m_DeviceHandle, m_FrameFlightFences[i], nullptr);
            vkDestroySemaphore(m_DeviceHandle, m_RenderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(m_DeviceHandle, m_ImageAvailableSemaphores[i], nullptr);
        }
        vkDestroyBuffer(m_DeviceHandle, m_TestBuffer, nullptr);
        for (auto &framebuffer : m_Framebuffers)
        {
            vkDestroyFramebuffer(m_DeviceHandle, framebuffer, nullptr);
        }
        vkDestroyPipelineLayout(m_DeviceHandle, m_PipelineLayout, nullptr);
        vkDestroyPipeline(m_DeviceHandle, m_GraphicsPipeline, nullptr);
        vkDestroyRenderPass(m_DeviceHandle, m_RenderPass, nullptr);
        for (auto &view : m_SwapchainImageViews)
        {
            vkDestroyImageView(m_DeviceHandle, view, nullptr);
        }
        vkDestroySwapchainKHR(m_DeviceHandle, m_Swapchain, nullptr);
    }

    b8 RendererContext::DrawTestFrame()
    {
        RenderBegin();
        Render();
        RenderEnd();
        return true;
    }

    b8 RendererContext::RenderBegin()
    {
        vkWaitForFences(m_DeviceHandle, 1, &m_FrameFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);
        vkResetFences(m_DeviceHandle, 1, &m_FrameFlightFences[m_CurrentFrame]);

        vkAcquireNextImageKHR(m_DeviceHandle, m_Swapchain, UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &m_CurrentImageIndex);

        vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame], 0);

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;

        VkResult result = vkBeginCommandBuffer(m_CommandBuffers[m_CurrentFrame], &beginInfo);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to begin recording Vulkan Command Buffer");

        VkRenderPassBeginInfo renderBeginInfo = {};
        renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderBeginInfo.renderPass = m_RenderPass;
        renderBeginInfo.framebuffer = m_Framebuffers[m_CurrentImageIndex];
        renderBeginInfo.renderArea.offset = {0, 0};
        renderBeginInfo.renderArea.extent = m_SwapchainImageExtent;

        VkClearValue clearColor = {};
        clearColor.color = {0.1f, 0.1f, 0.8f, 1.0f};
        clearColor.depthStencil = {0.0f};

        renderBeginInfo.clearValueCount = 1;
        renderBeginInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(m_CommandBuffers[m_CurrentFrame], &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    b8 RendererContext::Render()
    {
        vkCmdBindPipeline(m_CommandBuffers[m_CurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);
        vkCmdDraw(m_CommandBuffers[m_CurrentFrame], 3, 1, 0, 0);
    }

    b8 RendererContext::RenderEnd()
    {
        vkCmdEndRenderPass(m_CommandBuffers[m_CurrentFrame]);

        VkResult result = vkEndCommandBuffer(m_CommandBuffers[m_CurrentFrame]);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to end recording of a Vulkan Command Buffer");

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {m_ImageAvailableSemaphores[m_CurrentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        VkSemaphore signalSemaphores[] = {m_RenderFinishedSemaphores[m_CurrentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_CommandBuffers[m_CurrentFrame];

        result = vkQueueSubmit(m_GraphicsQueueHandle, 1, &submitInfo, m_FrameFlightFences[m_CurrentFrame]);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to submit a Vulkan Command Buffer to the Graphics Queue");
        CX_INFO("QUEUE SUBMITTED SUCCESSFULLY");

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &m_RenderFinishedSemaphores[m_CurrentFrame];

        VkSwapchainKHR swapchains[] = {m_Swapchain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapchains;
        presentInfo.pImageIndices = &m_CurrentImageIndex;
        presentInfo.pResults = nullptr;

        vkQueuePresentKHR(m_PresentQueueHandle, &presentInfo);

        m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    // INITIALISATION

    VkSwapchainKHR RendererContext::CreateSwapchain(const std::unique_ptr<RendererBackend> &backend, u32 windowWidth, u32 windowHeight)
    {
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;

        DeviceSwapchainSupportDetails swapchainSupportDetails = backend->GetSwapchainSupportDetails();
        DeviceQueueDetails queueDetails = backend->GetQueueDetails();

        VkSurfaceFormatKHR surfaceFormat = ChooseSwapchainSurfaceFormat(swapchainSupportDetails.SurfaceFormats);
        VkPresentModeKHR presentMode = ChooseSwapchainPresentMode(swapchainSupportDetails.PresentModes);
        VkExtent2D extent = ChooseSwapchainExtents(swapchainSupportDetails.Capabilities, windowWidth, windowHeight);

        u32 imageCount = swapchainSupportDetails.Capabilities.minImageCount + 1;
        if (swapchainSupportDetails.Capabilities.maxImageCount > 0 && imageCount > swapchainSupportDetails.Capabilities.maxImageCount)
        {
            imageCount = swapchainSupportDetails.Capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = backend->GetSurface();
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        u32 queueIndices[2] = {queueDetails.GraphicsIndex, queueDetails.PresentIndex};
        if (queueDetails.GraphicsIndex != queueDetails.PresentIndex)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = swapchainSupportDetails.Capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        VkResult result = vkCreateSwapchainKHR(m_DeviceHandle, &createInfo, nullptr, &swapchain);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a Vulkan Swapchain");

        m_SwapchainImageExtent = extent;
        m_SwapchainImageFormat = surfaceFormat.format;

        return swapchain;
    }

    std::vector<VkImage> RendererContext::RetrieveSwapchainImages()
    {
        u32 imageCount;
        vkGetSwapchainImagesKHR(m_DeviceHandle, m_Swapchain, &imageCount, nullptr);
        std::vector<VkImage> images(imageCount);
        vkGetSwapchainImagesKHR(m_DeviceHandle, m_Swapchain, &imageCount, images.data());
        return images;
    }

    std::vector<VkImageView> RendererContext::CreateSwapchainImageViews()
    {
        std::vector<VkImageView> imageViews(m_SwapchainImages.size());
        for (size_t i = 0; i < imageViews.size(); i++)
        {
            VkImageViewCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = m_SwapchainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = m_SwapchainImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            VkResult result = vkCreateImageView(m_DeviceHandle, &createInfo, nullptr, &imageViews[i]);
            CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a Vulkan ImageView");
        }

        return imageViews;
    }

    VkRenderPass RendererContext::CreateRenderPass()
    {
        VkRenderPass renderPass;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;

        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = m_SwapchainImageFormat;
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

        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        VkResult result = vkCreateRenderPass(m_DeviceHandle, &renderPassInfo, nullptr, &renderPass);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a Vulkan Render Pass!");

        CX_INFO("Created RenderPass");

        return renderPass;
    }

    VkPipeline RendererContext::CreateGraphicsPipeline()
    {
        auto vertCode = ReadShader("../Cortex/assets/basic.vert.spv");
        auto fragCode = ReadShader("../Cortex/assets/basic.frag.spv");

        VkShaderModule vertModule = CreateShaderModule(vertCode);
        VkShaderModule fragModule = CreateShaderModule(fragCode);

        VkPipelineShaderStageCreateInfo shaderStages[2];
        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].module = vertModule;
        shaderStages[0].pName = "main";
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].pSpecializationInfo = nullptr;
        shaderStages[0].pNext = nullptr;
        shaderStages[0].flags = 0;

        shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].module = fragModule;
        shaderStages[1].pName = "main";
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].pSpecializationInfo = nullptr;
        shaderStages[1].pNext = nullptr;
        shaderStages[1].flags = 0;

        auto bindingDescription = Vertex::GetBindingDescription();
        auto attributeDescriptions = Vertex::GetAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<u32>(attributeDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
        inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
        inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)m_SwapchainImageExtent.width;
        viewport.height = (float)m_SwapchainImageExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor = {};
        scissor.extent = m_SwapchainImageExtent;
        scissor.offset = {0, 0};

        VkPipelineViewportStateCreateInfo viewportInfo = {};
        viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportInfo.scissorCount = 1;
        viewportInfo.pScissors = &scissor;
        viewportInfo.viewportCount = 1;
        viewportInfo.pViewports = &viewport;

        VkPipelineRasterizationStateCreateInfo rasterizerInfo = {};
        rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizerInfo.depthClampEnable = VK_FALSE;
        rasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizerInfo.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizerInfo.lineWidth = 1.0f;
        rasterizerInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizerInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizerInfo.depthBiasEnable = VK_FALSE;
        rasterizerInfo.depthBiasConstantFactor = 0.0f; // Optional
        rasterizerInfo.depthBiasClamp = 0.0f;          // Optional
        rasterizerInfo.depthBiasSlopeFactor = 0.0f;    // Optional

        VkPipelineMultisampleStateCreateInfo multisamplerInfo = {};
        multisamplerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisamplerInfo.sampleShadingEnable = VK_FALSE;
        multisamplerInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlending = {};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional

        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;            // Optional
        pipelineLayoutInfo.pSetLayouts = nullptr;         // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0;    // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

        VkResult result = vkCreatePipelineLayout(m_DeviceHandle, &pipelineLayoutInfo, nullptr, &m_PipelineLayout);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a Vulkan Pipeline Layout");

        VkGraphicsPipelineCreateInfo createInfo = {};
        // SHADER
        createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        createInfo.stageCount = 2;
        createInfo.pStages = shaderStages;
        // STATIC STATE
        createInfo.pVertexInputState = &vertexInputInfo;
        createInfo.pInputAssemblyState = &inputAssemblyInfo;
        createInfo.pViewportState = &viewportInfo;
        createInfo.pRasterizationState = &rasterizerInfo;
        createInfo.pMultisampleState = &multisamplerInfo;
        createInfo.pDepthStencilState = nullptr;
        createInfo.pColorBlendState = &colorBlending;
        createInfo.pDynamicState = nullptr;
        // FIXED FUNCTION
        createInfo.layout = m_PipelineLayout;
        createInfo.renderPass = m_RenderPass;
        createInfo.subpass = 0;

        VkPipeline pipeline;
        result = vkCreateGraphicsPipelines(m_DeviceHandle, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a Vulkan Graphics Pipeline");

        vkDestroyShaderModule(m_DeviceHandle, fragModule, nullptr);
        vkDestroyShaderModule(m_DeviceHandle, vertModule, nullptr);

        return pipeline;
    }

    std::vector<VkFramebuffer> RendererContext::CreateFramebuffers()
    {
        std::vector<VkFramebuffer> framebuffers(m_SwapchainImageViews.size());

        for (size_t i = 0; i < framebuffers.size(); i++)
        {
            VkImageView attachments[] = {
                m_SwapchainImageViews[i]};

            VkFramebufferCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            createInfo.renderPass = m_RenderPass;
            createInfo.attachmentCount = 1;
            createInfo.pAttachments = attachments;
            createInfo.width = m_SwapchainImageExtent.width;
            createInfo.height = m_SwapchainImageExtent.height;
            createInfo.layers = 1;

            VkResult result = vkCreateFramebuffer(m_DeviceHandle, &createInfo, nullptr, &framebuffers[i]);
            CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create Vulkan Framebuffer");
        }

        return framebuffers;
    }

    VkBuffer RendererContext::CreateVertexBuffer() {
        VkBuffer buffer;
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = sizeof(m_TestVertices[0]) * m_TestVertices.size();
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        VkResult result = vkCreateBuffer(m_DeviceHandle, &bufferInfo, nullptr, &buffer);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a Vertex Buffer!");

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(m_DeviceHandle, buffer, &memRequirements);

        

        return buffer;
        
    }

    std::vector<VkCommandBuffer> RendererContext::CreateCommandBuffers(const std::unique_ptr<RendererBackend> &backend)
    {
        std::vector<VkCommandBuffer> buffers(MAX_FRAMES_IN_FLIGHT);
        VkCommandPool pool = backend->GetCommandPool();

        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = pool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = static_cast<u32>(buffers.size());

        VkResult result = vkAllocateCommandBuffers(m_DeviceHandle, &allocInfo, buffers.data());
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to allocate Vulkan Command Buffers");
        return buffers;
    }

    void RendererContext::CreateSynchronisationObjects()
    {
        m_ImageAvailableSemaphores.resize(m_CommandBuffers.size());
        m_RenderFinishedSemaphores.resize(m_CommandBuffers.size());
        m_FrameFlightFences.resize(m_CommandBuffers.size());

        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        VkResult result;
        for (size_t i = 0; i < m_CommandBuffers.size(); i++)
        {
            result = vkCreateSemaphore(m_DeviceHandle, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]);
            CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a Vulkan Semaphore!");
            result = vkCreateSemaphore(m_DeviceHandle, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]);
            CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a Vulkan Semaphore!");
            result = vkCreateFence(m_DeviceHandle, &fenceInfo, nullptr, &m_FrameFlightFences[i]);
            CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a Vulkan Fence!");
        }
    }

    // HELPERS

    VkSurfaceFormatKHR RendererContext::ChooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR> surfaceFormats)
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

    VkPresentModeKHR RendererContext::ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR> presentModes)
    {
        for (const auto &presentMode : presentModes)
        {
            if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                CX_DEBUG("Present Mode: VK_PRESENT_MODE_MAILBOX_KHR");
                return presentMode;
            }
        }
        CX_DEBUG("Present Mode: VK_PRESENT_MODE_FIFO_KHR");
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D RendererContext::ChooseSwapchainExtents(const VkSurfaceCapabilitiesKHR capabilities, u32 windowWidth, u32 windowHeight)
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

    VkShaderModule RendererContext::CreateShaderModule(const std::vector<char> &code)
    {
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const u32 *>(code.data());

        VkShaderModule shader;
        VkResult result = vkCreateShaderModule(m_DeviceHandle, &createInfo, nullptr, &shader);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a Vulkan Shader Module");

        return shader;
    }

    std::vector<char> RendererContext::ReadShader(const std::string &path)
    {
        std::ifstream file(path, std::ios::ate | std::ios::binary);
        CX_DEBUGASSERT_MSG(file.is_open(), "Failed to open shader source!");
        size_t size = (size_t)file.tellg();
        std::vector<char> buff(size);
        file.seekg(0);
        file.read(buff.data(), size);
        file.close();
        return buff;
    }
}