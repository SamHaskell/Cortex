#pragma once

#include "Cortex/Graphics/RendererDevice.hpp"

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
        RendererSwapchain(const RendererSwapchainConfig &config, const VkDevice &device, const VkSurfaceKHR& surface, const DeviceQueueFamilies& queueDetails, const DeviceSwapchainSupportDetails& swapchainDetails);
        ~RendererSwapchain();

        inline const VkSwapchainKHR& GetSwapchain() { return m_Swapchain; }

        RendererSwapchain(const RendererSwapchain &) = delete;
        RendererSwapchain &operator=(const RendererSwapchain &) = delete;

    private:
        const VkDevice& m_DeviceHandle;
        
        VkFormat m_SwapchainFormat;
        VkPresentModeKHR m_SwapchainPresentMode;
        VkExtent2D m_SwapchainExtent;

        VkSwapchainKHR m_Swapchain;
        std::vector<VkImage> m_SwapchainImages;
        std::vector<VkImageView> m_SwapchainImageViews;

        VkSwapchainKHR CreateSwapchain(const VkSurfaceKHR &surface, const DeviceQueueFamilies& queueDetails, const DeviceSwapchainSupportDetails& swapchainDetails, const RendererSwapchainConfig &config);
        std::vector<VkImage> RetrieveSwapchainImages();
        std::vector<VkImageView> CreateSwapchainImageViews();

        VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR> surfaceFormats);
        VkPresentModeKHR ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR> presentModes);
        VkExtent2D ChooseSwapchainExtents(const VkSurfaceCapabilitiesKHR capabilities, u32 windowWidth, u32 windowHeight);

        void LogSwapchainDetails();
    };    

}