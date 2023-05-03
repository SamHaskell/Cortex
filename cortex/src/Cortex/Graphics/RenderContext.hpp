#pragma once

#include "Cortex/Utils/Asserts.h"
#include "Cortex/Graphics/RenderDevice.hpp"
#include "Cortex/Graphics/Swapchain.hpp"

#include "vulkan/vulkan.h"

#include <vector>

namespace Cortex
{
    class RenderContext
    {
        public:
        RenderContext(const std::unique_ptr<RenderDevice>& renderDevice);
        ~RenderContext();

        void CreateSwapchain();
        void CreateRenderPass();

        private:
        std::unique_ptr<Swapchain> m_Swapchain;
        VkRenderPass m_RenderPass;

        VkImage m_DepthImage;
        VkImageView m_DepthImageView;
        std::vector<VkFramebuffer> m_Framebuffers;
        std::vector<VkCommandBuffer> m_CommandBuffers;
        std::vector<VkSemaphore> m_GraphicsSemaphores;
        std::vector<VkSemaphore> m_PresentSemaphores;

        VkViewport m_Viewport;
        VkRect2D m_Scissor;
        u32 currentFrameIndex;
        u32 currentImageIndex;
        
    };
}