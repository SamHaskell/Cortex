#pragma once

#include "Badger/Graphics/VulkanHelpers.hpp"
#include "Badger/Graphics/VulkanTypes.hpp"
#include "Badger/Graphics/RenderPass.hpp"
#include "Badger/Graphics/GraphicsDevice.hpp"

namespace Badger {
    class Swapchain {
        public:
            static std::unique_ptr<Swapchain> Create(std::shared_ptr<GraphicsDevice> device, VulkanSwapchainSpecification spec, RenderPass renderPass);
            Swapchain(std::shared_ptr<GraphicsDevice> device, VulkanSwapchainSpecification spec, RenderPass renderPass);
            ~Swapchain();
            Swapchain(const Swapchain&) = delete;
            Swapchain &operator=(const Swapchain&) = delete;
            VkResult SwapBuffers(const VkFence& inFlightFence, const VkSemaphore& imageAvailableSemaphore);
            VkResult PresentImage(const VkQueue& presentQueue, const VkSemaphore& renderFinishSemaphore);
            inline VkFramebuffer GetCurrentFramebuffer() { return m_SwapchainFramebuffers[m_CurrentImageIndex]; }
        private:
            u32 m_CurrentImageIndex;
            std::shared_ptr<GraphicsDevice> m_GraphicsDevice;
            VkSwapchainKHR m_SwapchainHandle;
            VulkanSwapchainSpecification m_Spec;
            std::vector<VkImage> m_SwapchainImages;
            std::vector<VkImageView> m_SwapchainImageViews;
            std::vector<VkFramebuffer> m_SwapchainFramebuffers;

    };
}