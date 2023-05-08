#include "Cortex/Graphics/RendererDevice.hpp"
#include "Cortex/Utils/Asserts.h"
#include "vulkan/vulkan.h"

namespace Cortex
{
    RendererDeviceConfig RendererDeviceConfig::Default()
    {
        RendererDeviceConfig config;
        config.DeviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            "VK_KHR_portability_subset"};
        config.RequireDiscreteGPU = false;
        config.RequireComputeQueue = false;
        return config;
    }

    b8 DeviceQueueFamilies::IsComplete(b8 computeRequired)
    {
        b8 result = (GraphicsFamily != UnsetIndex && PresentFamily != UnsetIndex);
        return result && (computeRequired ? ComputeFamily != UnsetIndex : true);
    }

    RendererDevice::RendererDevice(const RendererDeviceConfig &config, const VkInstance &instance, const VkSurfaceKHR &surface)
    : m_PhysicalDevice(ChoosePhysicalDevice(config, instance, surface))
    {
        LogPhysicalDeviceDetails();
    }

    VkPhysicalDevice RendererDevice::ChoosePhysicalDevice(const RendererDeviceConfig &config, const VkInstance& instance, const VkSurfaceKHR &surface)
    {
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

        u32 deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        CX_ASSERT_MSG(deviceCount != 0, "Failed to find any devices that support Vulkan!");
        std::vector<VkPhysicalDevice> availableDevices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, availableDevices.data());

        for (const auto &device : availableDevices)
        {
            if (CheckDeviceSuitable(device, surface, config))
            {
                physicalDevice = device;
                break;
            }
        }
        CX_ASSERT_MSG(physicalDevice != VK_NULL_HANDLE, "Failed to find a suitable Vulkan physical device!");
        return physicalDevice;
    }

    b8 RendererDevice::CheckDeviceSuitable(const VkPhysicalDevice device, const VkSurfaceKHR surface, const RendererDeviceConfig &config)
    {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);
        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(device, &features);

        DeviceQueueFamilies queueDetails = QueryQueueFamilies(device, surface, config.RequireComputeQueue);

        b8 isDiscrete = (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
        b8 hasFeatures = features.geometryShader;
        b8 hasExtensions = CheckDeviceExtensionSupport(device, config.DeviceExtensions);

        b8 swapchainSupport = false;
        if (hasExtensions)
        {
            DeviceSwapchainSupportDetails swapchainDetails = QuerySwapchainSupportDetails(device, surface);
            swapchainSupport = !swapchainDetails.PresentModes.empty() && !swapchainDetails.SurfaceFormats.empty();
        }

        b8 suitable = hasExtensions && queueDetails.IsComplete(config.RequireComputeQueue) && swapchainSupport;
        if (config.RequireDiscreteGPU)
        {
            suitable = suitable && isDiscrete;
        }

        return suitable;
    }

    b8 RendererDevice::CheckDeviceExtensionSupport(const VkPhysicalDevice device, const std::vector<const char *> requiredExtensions)
    {
        u32 count;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &count, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(count);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &count, availableExtensions.data());

        for (auto &name : requiredExtensions)
        {
            b8 found = false;
            for (auto &extension : availableExtensions)
            {
                if (strcmp(extension.extensionName, name) == 0)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                return false;
            }
        }
        return true;
    }

    DeviceQueueFamilies RendererDevice::QueryQueueFamilies(const VkPhysicalDevice device, const VkSurfaceKHR surface, b8 requireCompute)
    {
        DeviceQueueFamilies details;

        u32 queueFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProperties.data());

        u32 i = 0;
        for (const auto &properties : queueFamilyProperties)
        {
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
            if (presentSupport)
            {
                details.PresentFamily = i;
            }
            if (details.GraphicsFamily == UnsetIndex && properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                details.GraphicsFamily = i;
            }
            if (details.ComputeFamily == UnsetIndex && properties.queueFlags & VK_QUEUE_COMPUTE_BIT)
            {
                details.ComputeFamily = i;
            }
            if (details.IsComplete(requireCompute))
            {
                return details;
            }
            i++;
        }
        return details;
    }

    DeviceSwapchainSupportDetails RendererDevice::QuerySwapchainSupportDetails(const VkPhysicalDevice device, const VkSurfaceKHR surface)
    {
        DeviceSwapchainSupportDetails swapchainDetails;

        u32 formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
        if (formatCount != 0)
        {
            swapchainDetails.SurfaceFormats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, swapchainDetails.SurfaceFormats.data());
        }

        u32 presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
        swapchainDetails.PresentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, swapchainDetails.PresentModes.data());

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &swapchainDetails.Capabilities);

        return swapchainDetails;
    }

    void RendererDevice::LogPhysicalDeviceDetails()
    {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(m_PhysicalDevice, &props);
        const char *type;
        if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            type = "Discrete GPU";
        }
        else if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        {
            type = "Integrated GPU";
        }
        else if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU)
        {
            type = "CPU";
        }
        else
        {
            type = "Other";
        }
        CX_INFO("(Vulkan Physical Device) NAME: %s, ID: %u, TYPE: %s", props.deviceName, props.deviceID, type);
    }

}