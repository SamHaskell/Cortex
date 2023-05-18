#include "Cortex/Rendering/RenderDevice.hpp"

#include <set>

namespace Cortex
{
    RenderDeviceConfig RenderDeviceConfig::Default()
    {
        RenderDeviceConfig config;
        config.DeviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            "VK_KHR_portability_subset"};
        config.RequireDiscreteGPU = false;
        config.RequireComputeQueue = false;
        return config;
    }

    b8 DeviceQueueFamilies::IsComplete(b8 computeRequired)
    {
        b8 result = (GraphicsFamily != UnsetIndex && PresentFamily != UnsetIndex && TransferFamily != UnsetIndex);
        return result && (computeRequired ? ComputeFamily != UnsetIndex : true);
    }

    RenderDevice::RenderDevice(const RenderDeviceConfig &config, const std::unique_ptr<RenderInstance> &instance)
        : m_PhysicalDevice(ChoosePhysicalDevice(config, instance->GetInstance(), instance->GetSurface())),
          m_QueueFamilies(QueryQueueFamilies(m_PhysicalDevice, instance->GetSurface(), config.RequireComputeQueue)),
          m_SwapchainSupportDetails(QuerySwapchainSupportDetails(m_PhysicalDevice, instance->GetSurface())),
          m_Device(CreateDeviceAndQueues(config))
    {
        LogPhysicalDeviceDetails();
    }

    RenderDevice::~RenderDevice()
    {
        // m_DeletionQueue.Flush();
        vkDeviceWaitIdle(m_Device);
        vkDestroyDevice(m_Device, nullptr);
    }

    VkPhysicalDevice RenderDevice::ChoosePhysicalDevice(const RenderDeviceConfig &config, const VkInstance &instance, const VkSurfaceKHR &surface)
    {
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

        u32 deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        CX_ASSERT_MSG(deviceCount != 0, "Failed to find any devices that support Vulkan!");
        std::vector<VkPhysicalDevice> availableDevices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, availableDevices.data());

        for (const auto &device : availableDevices)
        {
            if (CheckDeviceSuitable(config, device, surface))
            {
                physicalDevice = device;
                break;
            }
        }
        CX_ASSERT_MSG(physicalDevice != VK_NULL_HANDLE, "Failed to find a suitable Vulkan physical device!");
        return physicalDevice;
    }

    VkDevice RenderDevice::CreateDeviceAndQueues(const RenderDeviceConfig &config)
    {
        VkDevice device;

        VkPhysicalDeviceFeatures features = {};

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<u32> uniqueQueueFamilies = {m_QueueFamilies.GraphicsFamily, m_QueueFamilies.PresentFamily};

        if (config.RequireComputeQueue)
        {
            uniqueQueueFamilies.emplace(m_QueueFamilies.ComputeFamily);
        }

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
        createInfo.enabledExtensionCount = static_cast<u32>(config.DeviceExtensions.size());
        createInfo.ppEnabledExtensionNames = config.DeviceExtensions.data();

        VkResult result = vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &device);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a Vulkan Device");

        vkGetDeviceQueue(device, m_QueueFamilies.GraphicsFamily, 0, &m_GraphicsQueue);
        vkGetDeviceQueue(device, m_QueueFamilies.PresentFamily, 0, &m_PresentQueue);
        vkGetDeviceQueue(device, m_QueueFamilies.TransferFamily, 0, &m_TransferQueue);

        if (config.RequireComputeQueue)
        {
            vkGetDeviceQueue(device, m_QueueFamilies.ComputeFamily, 0, &m_ComputeQueue);
        }

        return device;
    }

    b8 RenderDevice::CheckDeviceSuitable(const RenderDeviceConfig &config, const VkPhysicalDevice device, const VkSurfaceKHR surface)
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

    b8 RenderDevice::CheckDeviceExtensionSupport(const VkPhysicalDevice device, const std::vector<const char *> requiredExtensions)
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

    DeviceQueueFamilies RenderDevice::QueryQueueFamilies(const VkPhysicalDevice device, const VkSurfaceKHR surface, b8 requireCompute)
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
            if (details.TransferFamily == UnsetIndex && properties.queueFlags & VK_QUEUE_TRANSFER_BIT)
            {
                details.TransferFamily = i;
            }
            if (details.IsComplete(requireCompute))
            {
                return details;
            }
            i++;
        }
        return details;
    }

    DeviceSwapchainSupportDetails RenderDevice::QuerySwapchainSupportDetails(const VkPhysicalDevice device, const VkSurfaceKHR surface)
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

    void RenderDevice::LogPhysicalDeviceDetails()
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
        CX_INFO("(Vulkan Queue Indices) GRAPHICS: %u, PRESENT: %u, COMPUTE: %u, TRANSFER: %u", m_QueueFamilies.GraphicsFamily, m_QueueFamilies.PresentFamily, m_QueueFamilies.ComputeFamily, m_QueueFamilies.TransferFamily);
    }

    u32 RenderDevice::FindMemoryType(u32 typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memoryProperties);
        for (u32 i = 0; i < memoryProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties){
                return i;
            }
        }
        CX_ASSERT_MSG(false, "Failed to find a suitable device memory type!");
    }
}