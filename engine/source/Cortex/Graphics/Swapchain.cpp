#include "Cortex/Graphics/Swapchain.hpp"

namespace Cortex {
    std::unique_ptr<Swapchain> Swapchain::Create(std::shared_ptr<GraphicsDevice> device, VulkanSwapchainSpecification spec, RenderPass renderPass) {
        return std::make_unique<Swapchain>(device, spec, renderPass);
    }

    Swapchain::Swapchain(std::shared_ptr<GraphicsDevice> device, VulkanSwapchainSpecification spec, RenderPass renderPass) {
        m_GraphicsDevice = device;
        vulkan_create_swapchain(device->PhysicalDevice, device->Device, device->Surface, spec, m_SwapchainHandle);
        vulkan_get_swapchain_images(device->Device, m_SwapchainHandle, m_SwapchainImages);
        vulkan_create_swapchain_image_views(device->Device, spec, m_SwapchainImages, m_SwapchainImageViews);
        vulkan_create_depth_resources(device, spec, m_DepthResources);
        vulkan_create_color_resources(device, spec, m_ColorResources);
        vulkan_create_swapchain_framebuffers(device->Device, spec, m_SwapchainImageViews, m_DepthResources.ImageView, m_ColorResources.ImageView, renderPass.Pass, m_SwapchainFramebuffers);
    }

    Swapchain::~Swapchain() {
        vkDeviceWaitIdle(m_GraphicsDevice->Device);
        for (auto buffer: m_SwapchainFramebuffers) {
            vkDestroyFramebuffer(m_GraphicsDevice->Device, buffer, nullptr);
        }
        for (auto view : m_SwapchainImageViews) {
            vkDestroyImageView(m_GraphicsDevice->Device, view, nullptr);
        }
        vkDestroySwapchainKHR(m_GraphicsDevice->Device, m_SwapchainHandle, nullptr);
        vulkan_destroy_depth_resources(m_GraphicsDevice, m_DepthResources);
        vulkan_destroy_color_resources(m_GraphicsDevice, m_ColorResources);
    }

    VkResult Swapchain::SwapBuffers(const VkFence& inFlightFence, const VkSemaphore& imageAvailableSemaphore) {
        vkWaitForFences(m_GraphicsDevice->Device, 1, &inFlightFence, VK_TRUE, std::numeric_limits<u64>::max());
        VkResult result = vkAcquireNextImageKHR(
            m_GraphicsDevice->Device, m_SwapchainHandle, 
            std::numeric_limits<u64>::max(), 
            imageAvailableSemaphore, 
            VK_NULL_HANDLE, 
            &m_CurrentImageIndex);
        return result; 
    }

    VkResult Swapchain::PresentImage(const VkQueue& presentQueue, const VkSemaphore& renderFinishSemaphore) {
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &renderFinishSemaphore;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_SwapchainHandle;
        presentInfo.pImageIndices = &m_CurrentImageIndex;
        presentInfo.pResults = nullptr;
        VkResult result = vkQueuePresentKHR(presentQueue, &presentInfo);
        return result;
    }

    void vulkan_create_depth_resources(const std::shared_ptr<GraphicsDevice> device, VulkanSwapchainSpecification spec, VulkanDepthResources& depthResources) {
        vulkan_create_image(
            device->Device,
            device->PhysicalDevice,
            spec.Extent.width,
            spec.Extent.height,
            device->Details.MaxMultisamplingCount,
            device->Details.DepthFormat,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            depthResources.Image,
            depthResources.ImageMemory
        );

        depthResources.ImageView = vulkan_create_image_view(device->Device, depthResources.Image, device->Details.DepthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
    }

    void vulkan_create_color_resources(const std::shared_ptr<GraphicsDevice> device, VulkanSwapchainSpecification spec, VulkanColorResources& colorResources) {
        VkFormat format = spec.SurfaceFormat.format;
        
        vulkan_create_image(
            device->Device,
            device->PhysicalDevice,
            spec.Extent.width,
            spec.Extent.height,
            device->Details.MaxMultisamplingCount,
            format,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            colorResources.Image,
            colorResources.ImageMemory
        );

        colorResources.ImageView = vulkan_create_image_view(device->Device, colorResources.Image, format, VK_IMAGE_ASPECT_COLOR_BIT);
    }

    void vulkan_destroy_depth_resources(const std::shared_ptr<GraphicsDevice> device, VulkanDepthResources& depthResources) {
        vkDestroyImageView(device->Device, depthResources.ImageView, nullptr);
        vkFreeMemory(device->Device, depthResources.ImageMemory, nullptr);
        vkDestroyImage(device->Device, depthResources.Image, nullptr);
    }

    void vulkan_destroy_color_resources(const std::shared_ptr<GraphicsDevice> device, VulkanColorResources& colorResources) {
        vkDestroyImageView(device->Device, colorResources.ImageView, nullptr);
        vkFreeMemory(device->Device, colorResources.ImageMemory, nullptr);
        vkDestroyImage(device->Device, colorResources.Image, nullptr);
    }

}