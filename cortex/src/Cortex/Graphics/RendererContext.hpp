#pragma once

#include "Cortex/Utils/Asserts.h"

#include "Cortex/Graphics/RendererBackend.hpp"

namespace Cortex
{
    class RendererContext
    {
    public:
        RendererContext(const std::unique_ptr<RendererBackend> &backend, u32 windowWidth, u32 windowHeight);
        ~RendererContext();

    private:
        VkSwapchainKHR CreateSwapchain(const std::unique_ptr<RendererBackend> &backend, u32 windowWidth, u32 windowHeight);


        VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR> surfaceFormats);
        VkPresentModeKHR ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR> presentModes);
        VkExtent2D ChooseSwapchainExtents(const VkSurfaceCapabilitiesKHR capabilities, u32 windowWidth, u32 windowHeight);

    private:
        const VkDevice& m_DeviceHandle;
        VkSwapchainKHR m_Swapchain;

        std::vector<VkImage> m_SwapchainImages;
        std::vector<VkImageView> m_SwapchainImageViews;

        std::vector<VkFramebuffer> m_Framebuffers;
        std::vector<VkCommandBuffer> m_CommandBuffers;
    };
}