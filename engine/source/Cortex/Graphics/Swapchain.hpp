#pragma once

#include "Cortex/Graphics/VulkanHelpers.hpp"
#include "Cortex/Graphics/VulkanTypes.hpp"
#include "Cortex/Graphics/RenderPass.hpp"
#include "Cortex/Graphics/GraphicsDevice.hpp"

namespace Cortex {
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
            VulkanDepthResources m_DepthResources;
            VulkanColorResources m_ColorResources;
            std::vector<VkImage> m_SwapchainImages;
            std::vector<VkImageView> m_SwapchainImageViews;
            std::vector<VkFramebuffer> m_SwapchainFramebuffers;
    };

    void vulkan_create_depth_resources(const std::shared_ptr<GraphicsDevice> device, VulkanSwapchainSpecification spec, VulkanDepthResources& depthResources);
    void vulkan_create_color_resources(const std::shared_ptr<GraphicsDevice> device, VulkanSwapchainSpecification spec, VulkanColorResources& colorResources);
    void vulkan_destroy_depth_resources(const std::shared_ptr<GraphicsDevice> device, VulkanDepthResources& depthResources);
    void vulkan_destroy_color_resources(const std::shared_ptr<GraphicsDevice> device, VulkanColorResources& colorResources);
}