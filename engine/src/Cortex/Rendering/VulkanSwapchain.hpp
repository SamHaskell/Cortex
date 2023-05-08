#pragma once

#include "Cortex/Rendering/VulkanDevice.hpp"

#include "vulkan/vulkan.h"

#include <vector>

namespace Cortex
{
    class VulkanSwapchain
    {
    public:
        VulkanSwapchain(const std::unique_ptr<VulkanDevice> &device, const std::unique_ptr<VulkanInstance> &instance, u32 framebufferWidth, u32 framebufferHeight);
        ~VulkanSwapchain();

    private:
        VkSwapchainKHR CreateSwapchain(const std::unique_ptr<VulkanDevice> &device, const std::unique_ptr<VulkanInstance> &instance, u32 framebufferWidth, u32 framebufferHeight);
        std::vector<VkImage> RetrieveSwapchainImages();
        std::vector<VkImageView> CreateSwapchainImageViews();

        VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR> surfaceFormats);
        VkPresentModeKHR ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR> presentModes);
        VkExtent2D ChooseSwapchainExtents(const VkSurfaceCapabilitiesKHR capabilities, u32 windowWidth, u32 windowHeight);

        void LogSwapchainDetails();

        const VkDevice &m_DeviceHandle;

        VkSwapchainKHR m_Swapchain;
        std::vector<VkImage> m_SwapchainImages;
        std::vector<VkImageView> m_SwapchainImageViews;

        VkPresentModeKHR m_SwapchainPresentMode;
        VkFormat m_SwapchainFormat;
        VkExtent2D m_SwapchainExtent;
    };
}