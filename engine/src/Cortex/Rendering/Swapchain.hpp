#pragma once

#include "Cortex/Rendering/VulkanUtils.h"
#include "Cortex/Utils/Asserts.h"

#include "Cortex/Rendering/RenderInstance.hpp"
#include "Cortex/Rendering/RenderDevice.hpp"

namespace Cortex
{
    struct SwapchainConfig
    {
        u32 FramebufferWidth;
        u32 FramebufferHeight;
        static SwapchainConfig Default(u32 width, u32 height);
    };

    class Swapchain
    {
        public:
        Swapchain(const SwapchainConfig& swapchainConfig, const std::unique_ptr<RenderInstance>& instance, const std::unique_ptr<RenderDevice>& device);
        ~Swapchain();

        inline const VkSwapchainKHR& GetSwapchain() { return m_Swapchain; }

        Swapchain(const Swapchain &) = delete;
        Swapchain &operator=(const Swapchain &) = delete;

        private:
        const VkDevice m_DeviceHandle;
        VkFormat m_SwapchainFormat;
        VkPresentModeKHR m_SwapchainPresentMode;
        VkExtent2D m_SwapchainExtent;

        VkSwapchainKHR m_Swapchain;
        std::vector<VkImage> m_SwapchainImages;
        std::vector<VkImageView> m_SwapchainImageViews;

        VkResult PresentImage(VkQueue queue, u32 imageIndex, VkSemaphore waitSemaphore);

        VkSwapchainKHR CreateSwapchain(const SwapchainConfig &config, const VkDevice& device, const VkSurfaceKHR &surface, const DeviceQueueFamilies& queueDetails, const DeviceSwapchainSupportDetails& swapchainDetails);
        std::vector<VkImage> RetrieveSwapchainImages(const VkDevice& device);
        std::vector<VkImageView> CreateSwapchainImageViews(const VkDevice& device);

        VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR> surfaceFormats);
        VkPresentModeKHR ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR> presentModes);
        VkExtent2D ChooseSwapchainExtents(const VkSurfaceCapabilitiesKHR capabilities, u32 windowWidth, u32 windowHeight);

        void LogSwapchainDetails();
    };
}