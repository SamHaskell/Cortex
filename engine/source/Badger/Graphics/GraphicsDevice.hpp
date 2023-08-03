#pragma once

#include "Badger/Graphics/VulkanHelpers.hpp"
#include "Badger/Graphics/VulkanTypes.hpp"

#include "Badger/Core/Window.hpp"

namespace Badger {
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
    };
}