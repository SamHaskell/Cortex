#pragma once

#include "Badger/Graphics/VulkanHelpers.hpp"
#include "Badger/Graphics/VulkanTypes.hpp"

namespace Badger {
    class Swapchain {
        public:
            static std::unique_ptr<Swapchain> Create(const VkPhysicalDevice& physicalDevice, VkDevice device, const VkSurfaceKHR& surface, VulkanSwapchainSpecification spec, VkRenderPass renderPass);
            Swapchain(const VkPhysicalDevice& physicalDevice, VkDevice device, const VkSurfaceKHR& surface, VulkanSwapchainSpecification spec, VkRenderPass renderPass);
            ~Swapchain();
            Swapchain(const Swapchain&) = delete;
            Swapchain &operator=(const Swapchain&) = delete;
            VkResult SwapBuffers(const VkFence& inFlightFence, const VkSemaphore& imageAvailableSemaphore);
            VkResult PresentImage(const VkQueue& presentQueue, const VkSemaphore& renderFinishSemaphore);
            inline VkFramebuffer GetCurrentFramebuffer() { return m_SwapchainFramebuffers[m_CurrentImageIndex]; }
        private:
            u32 m_CurrentImageIndex;
            VkDevice m_DeviceHandle;
            VkSwapchainKHR m_SwapchainHandle;
            VulkanSwapchainSpecification m_Spec;
            std::vector<VkImage> m_SwapchainImages;
            std::vector<VkImageView> m_SwapchainImageViews;
            std::vector<VkFramebuffer> m_SwapchainFramebuffers;

    };
}