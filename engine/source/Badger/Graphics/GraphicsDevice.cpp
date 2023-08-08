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

        Details.MaxMultisamplingCount = vulkan_get_max_msaa_count(PhysicalDevice);
        Details.DepthFormat = vulkan_find_supported_format(
            PhysicalDevice, 
            {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );
    }

    GraphicsDevice::~GraphicsDevice() {
        vkDeviceWaitIdle(Device);
        vkDestroyCommandPool(Device, TransferCommandPool, nullptr);
        vkDestroyDevice(Device, nullptr);
        vkDestroySurfaceKHR(Instance, Surface, nullptr);
        vkDestroyInstance(Instance, nullptr);  
    }
}