#include "Cortex/Graphics/RenderDevice.hpp"

#include <algorithm>
#include <cstring>
#include <set>
#include <unordered_set>

namespace Cortex
{
    RenderDevice::RenderDevice(const std::unique_ptr<RenderBackend>& backend, const std::unique_ptr<Window> &window) : m_InstanceHandle(backend->GetInstance())
    {
        CreateSurface(window);
        ChoosePhysicalDevice();
        CreateDevice();
        CreateCommandPool();
    }

    RenderDevice::~RenderDevice()
    {
        vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
        vkDestroyDevice(m_Device, nullptr);
        vkDestroySurfaceKHR(m_InstanceHandle, m_Surface, nullptr);
    }

    void RenderDevice::ChoosePhysicalDevice()
    {
        u32 deviceCount = 0;
        vkEnumeratePhysicalDevices(m_InstanceHandle, &deviceCount, nullptr);
        CX_ASSERT_MSG(deviceCount != 0, "No devices found that support Vulkan");
        CX_DEBUG("Physical Device count: %u", deviceCount);
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_InstanceHandle, &deviceCount, devices.data());

        for (const auto &device : devices)
        {
            if (IsDeviceSuitable(device, m_Surface))
            {
                m_PhysicalDevice = device;
                break;
            }
        }

        CX_ASSERT_MSG(m_PhysicalDevice != VK_NULL_HANDLE, "Could not find a suitable graphics device");

        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(m_PhysicalDevice, &properties);
        CX_DEBUG("Physical Device Chosen: %s", properties.deviceName);
    }

    void RenderDevice::CreateDevice()
    {
        QueueInfo info = GetQueueInfo(m_PhysicalDevice, m_Surface);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        queueCreateInfos.push_back(FillQueueCreateInfo(1, info.GraphicsIndex, 1.0f));

        std::vector<const char *> deviceExtensions;
        deviceExtensions.push_back("VK_KHR_portability_subset");
        deviceExtensions.push_back("VK_KHR_swapchain");

        VkDeviceCreateInfo createInfo = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
        createInfo.pEnabledFeatures = &m_DeviceFeatures;
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        VkResult result = vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create Vulkan Logical Device");

        vkGetDeviceQueue(m_Device, info.GraphicsIndex, 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_Device, info.GraphicsIndex, 0, &m_PresentQueue);

        m_QueueInfo = info;
    }

    void RenderDevice::CreateCommandPool()
    {
        VkCommandPoolCreateInfo createInfo = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
        createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        createInfo.queueFamilyIndex = m_QueueInfo.GraphicsIndex;

        VkResult result = vkCreateCommandPool(m_Device, &createInfo, nullptr, &m_CommandPool);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create Vulkan Command Pool");
    }

    b8 RenderDevice::IsDeviceSuitable(const VkPhysicalDevice device, const VkSurfaceKHR surface)
    {

        QueueInfo info = GetQueueInfo(device, surface);
        b8 extensionsSupported = CheckDeviceExtensionSupport(device);

        b8 swapChainAdequate = false;
        if (extensionsSupported)
        {
            SwapchainSupportDetails swapChainSupport = QuerySwapchainSupport(device);
            swapChainAdequate = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentModes.empty();
        }

        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

        return info.IsComplete && extensionsSupported && swapChainAdequate &&
               supportedFeatures.samplerAnisotropy;
    }

    QueueInfo RenderDevice::GetQueueInfo(const VkPhysicalDevice device, const VkSurfaceKHR surface)
    {
        u32 familyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);
        std::vector<VkQueueFamilyProperties> familyProperties(familyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, familyProperties.data());

        QueueInfo indices = {};
        for (u32 i = 0; i < familyCount; i++)
        {
            // Check for Graphics Queue
            if (familyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && indices.GraphicsIndex == unsetIndex)
            {
                indices.GraphicsIndex = i;
            }

            // Check for Presentation Queue
            VkBool32 presentationSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentationSupport);
            if (presentationSupport && indices.PresentIndex == unsetIndex)
            {
                indices.PresentIndex = i;
            }

            // Check if we have found indices for all Queue Families.
            if (indices.IsComplete)
            {
                return indices;
            }
        }
        return indices;
    }

    b8 RenderDevice::CheckDeviceExtensionSupport(const VkPhysicalDevice device)
    {
        u32 extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(
            device,
            nullptr,
            &extensionCount,
            availableExtensions.data());

        std::set<std::string> requiredExtensions(m_Extensions.begin(), m_Extensions.end());

        for (const auto &extension : availableExtensions)
        {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    SwapchainSupportDetails RenderDevice::QuerySwapchainSupport(const VkPhysicalDevice device)
    {
        SwapchainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.Capabilities);

        u32 formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);

        if (formatCount != 0)
        {
            details.Formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.Formats.data());
        }

        u32 presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, nullptr);

        if (presentModeCount != 0)
        {
            details.PresentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                device,
                m_Surface,
                &presentModeCount,
                details.PresentModes.data());
        }
        return details;
    }

    VkDeviceQueueCreateInfo RenderDevice::FillQueueCreateInfo(uint32_t count, uint32_t queueFamilyIndex, float priority)
    {
        VkDeviceQueueCreateInfo queueInfo;
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueCount = count;
        queueInfo.queueFamilyIndex = queueFamilyIndex;
        queueInfo.flags = 0;
        queueInfo.pQueuePriorities = &priority;
        queueInfo.pNext = nullptr;
        return queueInfo;
    }
}