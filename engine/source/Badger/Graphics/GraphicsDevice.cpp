#include "Badger/Graphics/GraphicsDevice.hpp"

namespace Badger {
    std::unique_ptr<GraphicsDevice> GraphicsDevice::Create(VulkanSessionConfig config, const std::unique_ptr<Window>& window) {
        return std::make_unique<GraphicsDevice>(config, window);
    }

    GraphicsDevice::GraphicsDevice(VulkanSessionConfig config, const std::unique_ptr<Window>& window) {
        vulkan_create_instance(config.ValidationLayers, Instance);
        window->GetVulkanSurface(Instance, Surface);
        VulkanPhysicalDeviceRequirements deviceRequirements = {
            config.DeviceExtensions,
            false,
            false,
            true,
            true
        };
        vulkan_obtain_physical_device(Instance, Surface, deviceRequirements, PhysicalDevice);
        vulkan_create_device(
            Instance, 
            PhysicalDevice, 
            Surface,
            config.DeviceExtensions, 
            Device, 
            QueueIndices, 
            Queues
        );
        TransferCommandPool = vulkan_create_command_pool(Device, QueueIndices.Transfer, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
    }

    GraphicsDevice::~GraphicsDevice() {
        vkDeviceWaitIdle(Device);
        vkDestroyCommandPool(Device, TransferCommandPool, nullptr);
        vkDestroyDevice(Device, nullptr);
        vkDestroySurfaceKHR(Instance, Surface, nullptr);
        vkDestroyInstance(Instance, nullptr);  
    }
}