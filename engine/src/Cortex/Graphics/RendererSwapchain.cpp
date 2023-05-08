#include "Cortex/Graphics/RendererSwapchain.hpp"

#include "vulkan/vulkan.h"

namespace Cortex
{
    RendererSwapchainConfig RendererSwapchainConfig::Default(u32 width, u32 height)
    {
        RendererSwapchainConfig config;
        config.FramebufferWidth = width;
        config.FramebufferHeight = height;
        return config;
    }

    RendererSwapchain::RendererSwapchain(const RendererSwapchainConfig &config)
    {
        
    }
}