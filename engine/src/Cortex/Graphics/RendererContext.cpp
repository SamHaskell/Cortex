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
          m_Swapchain({window.GetFramebufferWidth(), window.GetFramebufferHeight()})
    {
    }

    b8 RendererContext::BeginFrame()
    {
        return true;
    }

    b8 RendererContext::EndFrame()
    {
        m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
        return true;
    }

}