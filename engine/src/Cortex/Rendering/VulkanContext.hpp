#pragma once

#include "vulkan/vulkan.h"

#include "Cortex/Rendering/VulkanInstance.hpp"
#include "Cortex/Rendering/VulkanDevice.hpp"
#include "Cortex/Rendering/VulkanSwapchain.hpp"
#include "Cortex/Rendering/VulkanRenderer.hpp"

#include <vector>
#include <memory>

namespace Cortex {
    class VulkanContext {
        public:
        VulkanContext(std::unique_ptr<Window> &window);
        ~VulkanContext();
        private:
        std::unique_ptr<VulkanInstance> m_Instance;
        std::unique_ptr<VulkanDevice> m_Device;
        std::unique_ptr<VulkanSwapchain> m_Swapchain;

        std::unique_ptr<VulkanRenderer> m_Renderer;
        std::vector<VkCommandBuffer> m_CommandBuffers;
        std::vector<VkSemaphore> m_RenderCompleteSemaphores;
        std::vector<VkSemaphore> m_ImageAvailableSemaphores;
        std::vector<VkFence> m_FlightFence;
    };
}