#include "Cortex/Rendering/VulkanContext.hpp"

namespace Cortex
{
    VulkanContext::VulkanContext(std::unique_ptr<Window> &window)
    {
        m_Instance = std::make_unique<VulkanInstance>(window);
        m_Device = std::make_unique<VulkanDevice>(m_Instance);
        m_Swapchain = std::make_unique<VulkanSwapchain>(m_Device, m_Instance, window->GetFramebufferWidth(), window->GetFramebufferHeight());
    }

    VulkanContext::~VulkanContext()
    {
    }
}