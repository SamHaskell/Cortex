#pragma once

#include "Cortex/Graphics/VulkanHelpers.hpp"
#include "Cortex/Graphics/VulkanTypes.hpp"

#include "Cortex/Core/Window.hpp"

namespace Cortex {
    class GraphicsDevice {
        public:
            static std::unique_ptr<GraphicsDevice> Create(VulkanSessionConfig config, const std::unique_ptr<Window>& window);
            GraphicsDevice(VulkanSessionConfig config, const std::unique_ptr<Window>& window);
            ~GraphicsDevice();
            GraphicsDevice(const GraphicsDevice&) = delete;
            GraphicsDevice &operator=(const GraphicsDevice&) = delete;

            VkInstance Instance;
            VkSurfaceKHR Surface;
            VkPhysicalDevice PhysicalDevice;
            VkDevice Device;
            VulkanQueueIndices QueueIndices;
            VulkanQueues Queues;
            VkCommandPool TransferCommandPool;
            VulkanDeviceDetails Details;
    };
}