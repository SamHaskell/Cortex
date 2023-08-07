#include "Badger/Graphics/Swapchain.hpp"

namespace Badger {
    std::unique_ptr<Swapchain> Swapchain::Create(std::shared_ptr<GraphicsDevice> device, VulkanSwapchainSpecification spec, RenderPass renderPass) {
        return std::make_unique<Swapchain>(device, spec, renderPass);
    }

    Swapchain::Swapchain(std::shared_ptr<GraphicsDevice> device, VulkanSwapchainSpecification spec, RenderPass renderPass) {
        m_GraphicsDevice = device;
        vulkan_create_swapchain(device->PhysicalDevice, device->Device, device->Surface, spec, m_SwapchainHandle);
        vulkan_get_swapchain_images(device->Device, m_SwapchainHandle, m_SwapchainImages);
        vulkan_create_swapchain_image_views(device->Device, spec, m_SwapchainImages, m_SwapchainImageViews);
        vulkan_create_swapchain_framebuffers(device->Device, spec, m_SwapchainImageViews, renderPass.Pass, m_SwapchainFramebuffers);
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

}