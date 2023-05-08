#pragma once

#include "Cortex/Rendering/VulkanInstance.hpp"

#include "vulkan/vulkan.h"
#include <vector>

namespace Cortex
{
    constexpr u32 UnsetIndex{std::numeric_limits<u32>::max()};

    struct DeviceQueueDetails
    {
        u32 GraphicsIndex = UnsetIndex;
        u32 PresentIndex = UnsetIndex;
        inline b8 IsComplete() { return GraphicsIndex != UnsetIndex && PresentIndex != UnsetIndex; }
    };

    struct DeviceSwapchainSupportDetails
    {
        std::vector<VkSurfaceFormatKHR> SurfaceFormats;
        std::vector<VkPresentModeKHR> PresentModes;
        VkSurfaceCapabilitiesKHR Capabilities;
    };

    class VulkanDevice
    {
    public:
        VulkanDevice(std::unique_ptr<VulkanInstance> &instance);
        ~VulkanDevice();

        inline const VkPhysicalDevice& GetPhysicalDevice() { return m_PhysicalDevice; }
        inline const VkDevice& GetDevice() { return m_Device; }
        inline const DeviceQueueDetails& GetQueueDetails() { return m_QueueDetails; }
        inline const DeviceSwapchainSupportDetails& GetSwapchainSupportDetails() { return m_SwapchainSupportDetails; }

    private:
        VkPhysicalDevice ChoosePhysicalDevice(std::unique_ptr<VulkanInstance> &instance);
        VkDevice CreateDeviceAndQueues();
        VkCommandPool CreateCommandPool(const u32 queueIndex);

        b8 CheckDeviceSuitable(const VkPhysicalDevice device, const VkSurfaceKHR surface, const std::vector<const char *> requiredExtensions);
        b8 CheckDeviceExtensionSupport(const VkPhysicalDevice device, const std::vector<const char *> requiredExtensions);
        DeviceQueueDetails QueryQueueDetails(const VkPhysicalDevice device, const VkSurfaceKHR surface);
        DeviceSwapchainSupportDetails QuerySwapchainSupportDetails(const VkPhysicalDevice device, const VkSurfaceKHR surface);

        void LogPhysicalDeviceDetails();

        const std::vector<const char *> m_RequiredExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            "VK_KHR_portability_subset"};

        const VkPhysicalDevice m_PhysicalDevice;
        const DeviceQueueDetails m_QueueDetails;
        const DeviceSwapchainSupportDetails m_SwapchainSupportDetails;
        const VkDevice m_Device;
        VkQueue m_GraphicsQueue;
        VkQueue m_PresentQueue;
        VkCommandPool m_CommandPool;
    };
}