#pragma once

#include "Badger/Graphics/VulkanHelpers.hpp"
#include "Badger/Graphics/VulkanTypes.hpp"

#include "Badger/Core/Window.hpp"

namespace Badger {
    class GraphicsDevice {
        public:
            GraphicsDevice(VulkanSessionConfig config, std::unique_ptr<Window> window);
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
    };
}