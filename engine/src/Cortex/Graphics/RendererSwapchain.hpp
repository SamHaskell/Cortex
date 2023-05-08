#pragma once

#include "Cortex/Utils/Defines.h"

#include "vulkan/vulkan.h"
#include <vector>

namespace Cortex
{
    struct RendererSwapchainConfig
    {
        u32 FramebufferWidth;
        u32 FramebufferHeight;
        static RendererSwapchainConfig Default(u32 width, u32 height);
    };

    class RendererSwapchain
    {
    public:
        RendererSwapchain(const RendererSwapchainConfig &config);
        void DestroySwapchain();

        RendererSwapchain(const RendererSwapchain &) = delete;
        RendererSwapchain &operator=(const RendererSwapchain &) = delete;

    private:
        VkSwapchainKHR m_Swapchain;
        std::vector<VkImage> m_SwapchainImages;
        std::vector<VkImageView> m_SwapchainImageViews;

        VkSwapchainKHR CreateSwapchain();
    };    

}