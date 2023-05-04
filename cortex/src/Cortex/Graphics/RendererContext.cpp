#include "Cortex/Graphics/RendererContext.hpp"

#include "Cortex/Core/Window.hpp"

namespace Cortex
{
    RendererContext::RendererContext(const std::unique_ptr<RendererBackend> &backend, u32 windowWidth, u32 windowHeight)
        : m_DeviceHandle(backend->GetDevice()),
          m_Swapchain(CreateSwapchain(backend, windowWidth, windowHeight))
    {
    }

    RendererContext::~RendererContext()
    {
        vkDestroySwapchainKHR(m_DeviceHandle, m_Swapchain, nullptr);
    }

    VkSwapchainKHR RendererContext::CreateSwapchain(const std::unique_ptr<RendererBackend> &backend, u32 windowWidth, u32 windowHeight)
    {
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;

        DeviceSwapchainSupportDetails swapchainSupportDetails = backend->GetSwapchainSupportDetails();
        DeviceQueueDetails queueDetails = backend->GetQueueDetails();

        VkSurfaceFormatKHR surfaceFormat = ChooseSwapchainSurfaceFormat(swapchainSupportDetails.SurfaceFormats);
        VkPresentModeKHR presentMode = ChooseSwapchainPresentMode(swapchainSupportDetails.PresentModes);      
        VkExtent2D extent = ChooseSwapchainExtents(swapchainSupportDetails.Capabilities, windowWidth, windowHeight);  

        

        return swapchain;
    }

    VkSurfaceFormatKHR RendererContext::ChooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR> surfaceFormats)
    {
        for (const auto& surfaceFormat : surfaceFormats) {
            if (surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                if (surfaceFormat.format == VK_FORMAT_R8G8B8A8_SRGB || surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB) {
                    return surfaceFormat;
                }
                else if (surfaceFormat.format == VK_FORMAT_R8G8B8A8_UNORM || surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM) {
                    return surfaceFormat;
                }
            }
        }
        CX_WARN("No preferred format available, defaulting to first found");
        return surfaceFormats[0];
    }

    VkPresentModeKHR RendererContext::ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR> presentModes)
    {
        for (const auto& presentMode : presentModes) {
            if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                CX_DEBUG("Present Mode: VK_PRESENT_MODE_MAILBOX_KHR");
                return presentMode;
            }
        }
        CX_DEBUG("Present Mode: VK_PRESENT_MODE_FIFO_KHR");
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D RendererContext::ChooseSwapchainExtents(const VkSurfaceCapabilitiesKHR capabilities, u32 windowWidth, u32 windowHeight)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<u32>::max()) {
            return capabilities.currentExtent;
        } else {
            VkExtent2D trueExtent = {windowWidth, windowHeight};
            trueExtent.width = std::clamp(trueExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            trueExtent.width = std::clamp(trueExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return trueExtent;
        }
    }
}