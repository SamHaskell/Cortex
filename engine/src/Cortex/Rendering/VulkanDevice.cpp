#include "Cortex/Rendering/VulkanDevice.hpp"

#include <set>

namespace Cortex
{
    VulkanDevice::VulkanDevice(std::unique_ptr<VulkanInstance> &instance)
        : m_PhysicalDevice(ChoosePhysicalDevice(instance)),
          m_QueueDetails(QueryQueueDetails(m_PhysicalDevice, instance->GetSurface())),
          m_SwapchainSupportDetails(QuerySwapchainSupportDetails(m_PhysicalDevice, instance->GetSurface())),
          m_Device(CreateDeviceAndQueues()),
          m_CommandPool(CreateCommandPool(m_QueueDetails.GraphicsIndex))
    {
        LogPhysicalDeviceDetails();
    }

    VulkanDevice::~VulkanDevice()
    {
        vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
        vkDestroyDevice(m_Device, nullptr);
    }

    VkPhysicalDevice VulkanDevice::ChoosePhysicalDevice(std::unique_ptr<VulkanInstance> &instance)
    {
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

        u32 deviceCount = 0;
        vkEnumeratePhysicalDevices(instance->GetInstance(), &deviceCount, nullptr);
        CX_ASSERT_MSG(deviceCount != 0, "Failed to find any devices that support Vulkan!");
        std::vector<VkPhysicalDevice> availableDevices(deviceCount);
        vkEnumeratePhysicalDevices(instance->GetInstance(), &deviceCount, availableDevices.data());

        for (const auto &device : availableDevices)
        {
            if (CheckDeviceSuitable(device, instance->GetSurface(), m_RequiredExtensions))
            {
                physicalDevice = device;
                break;
            }
        }
        CX_ASSERT_MSG(physicalDevice != VK_NULL_HANDLE, "Failed to find a suitable Vulkan physical device!");
        return physicalDevice;
    }

    VkDevice VulkanDevice::CreateDeviceAndQueues()
    {
        VkDevice device;

        VkPhysicalDeviceFeatures features = {};

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<u32> uniqueQueueFamilies = {m_QueueDetails.GraphicsIndex, m_QueueDetails.PresentIndex};

        f32 queuePriority = 1.0f;
        for (u32 family : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.queueFamilyIndex = family;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<u32>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &features;
        createInfo.enabledExtensionCount = static_cast<u32>(m_RequiredExtensions.size());
        createInfo.ppEnabledExtensionNames = m_RequiredExtensions.data();

        VkResult result = vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &device);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a Vulkan Device");

        vkGetDeviceQueue(device, m_QueueDetails.GraphicsIndex, 0, &m_GraphicsQueue);
        vkGetDeviceQueue(device, m_QueueDetails.PresentIndex, 0, &m_PresentQueue);

        return device;
    }

    VkCommandPool VulkanDevice::CreateCommandPool(const u32 queueIndex)
    {
        VkCommandPool pool = VK_NULL_HANDLE;

        VkCommandPoolCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        createInfo.queueFamilyIndex = queueIndex;

        VkResult result = vkCreateCommandPool(m_Device, &createInfo, nullptr, &pool);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a command pool!");

        return pool;
    }

    b8 VulkanDevice::CheckDeviceSuitable(const VkPhysicalDevice device, const VkSurfaceKHR surface, const std::vector<const char *> requiredExtensions)
    {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);
        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(device, &features);

        DeviceQueueDetails queueDetails = QueryQueueDetails(device, surface);

        b8 isDiscrete = (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
        b8 hasFeatures = features.geometryShader;
        b8 hasExtensions = CheckDeviceExtensionSupport(device, requiredExtensions);

        b8 swapchainSupport = false;
        if (hasExtensions)
        {
            DeviceSwapchainSupportDetails swapchainDetails = QuerySwapchainSupportDetails(device, surface);
            swapchainSupport = !swapchainDetails.PresentModes.empty() && !swapchainDetails.SurfaceFormats.empty();
        }

        return hasExtensions && queueDetails.IsComplete() && swapchainSupport;
    }

    b8 VulkanDevice::CheckDeviceExtensionSupport(const VkPhysicalDevice device, const std::vector<const char *> requiredExtensions)
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

    DeviceQueueDetails VulkanDevice::QueryQueueDetails(const VkPhysicalDevice device, const VkSurfaceKHR surface)
    {
        DeviceQueueDetails details;

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
                details.PresentIndex = i;
            }
            if (details.GraphicsIndex == UnsetIndex && properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                details.GraphicsIndex = i;
            }
            if (details.IsComplete())
            {
                return details;
            }
            i++;
        }
        return details;
    }

    DeviceSwapchainSupportDetails VulkanDevice::QuerySwapchainSupportDetails(const VkPhysicalDevice device, const VkSurfaceKHR surface)
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

    void VulkanDevice::LogPhysicalDeviceDetails()
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