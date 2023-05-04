#include "Cortex/Graphics/RendererBackend.hpp"

#include "Cortex/Core/Window.hpp"

#include <set>

namespace Cortex
{
    RendererBackend::RendererBackend(const std::unique_ptr<Window> &window)
        : m_Instance(CreateInstance()),
          m_Surface(CreateSurface(*window)),
          m_PhysicalDevice(ChoosePhysicalDevice()),
          m_Device(CreateDevice()),
          m_CommandPool(CreateCommandPool())
    {
    }

    RendererBackend::~RendererBackend()
    {
        vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
        vkDestroyDevice(m_Device, nullptr);
        vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
        vkDestroyInstance(m_Instance, nullptr);
    }

    VkInstance RendererBackend::CreateInstance()
    {
        VkInstance instance;

        std::vector<const char *> layers = {
            "VK_LAYER_KHRONOS_validation"};

        b8 supported = CheckValidationLayerSupport(layers);
        CX_ASSERT_MSG(supported, "Requested Validation Layers not supported by the existing Vulkan Implementation");

        u32 count;
        const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&count);
        std::vector<const char *> extensions(count);
        for (u32 i = 0; i < count; i++)
        {
            extensions[i] = glfwExtensions[i];
        }
        extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Cortex Application";
        appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
        appInfo.pEngineName = "Cortex Engine";
        appInfo.engineVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        createInfo.enabledLayerCount = static_cast<u32>(layers.size());
        createInfo.ppEnabledLayerNames = layers.data();
        createInfo.enabledExtensionCount = static_cast<u32>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.pApplicationInfo = &appInfo;

        VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a Vulkan Instance!");

        return instance;
    }

    VkSurfaceKHR RendererBackend::CreateSurface(Window &window)
    {
        VkSurfaceKHR surface;
        window.CreateVulkanSurface(m_Instance, surface);
        return surface;
    }

    VkPhysicalDevice RendererBackend::ChoosePhysicalDevice()
    {
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

        u32 deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);
        CX_ASSERT_MSG(deviceCount != 0, "Failed to find any devices that support Vulkan!");
        std::vector<VkPhysicalDevice> availableDevices(deviceCount);
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, availableDevices.data());

        for (const auto &device : availableDevices)
        {
            if (CheckDeviceSuitable(device))
            {
                physicalDevice = device;
                break;
            }
        }

        CX_ASSERT_MSG(physicalDevice != VK_NULL_HANDLE, "Failed to find a suitable Vulkan physical device!");
        return physicalDevice;
    }

    VkDevice RendererBackend::CreateDevice()
    {
        VkDevice device;

        DeviceQueueDetails queueDetails = QueryQueueDetails(m_PhysicalDevice);

        VkPhysicalDeviceFeatures features = {};

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<u32> uniqueQueueFamilies = {queueDetails.GraphicsIndex, queueDetails.PresentIndex};

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
        createInfo.enabledExtensionCount = static_cast<u32>(m_DeviceExtensions.size());
        createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

        VkResult result = vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &device);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a Vulkan Device");

        return device;
    }

    VkCommandPool RendererBackend::CreateCommandPool()
    {
        VkCommandPool pool = VK_NULL_HANDLE;

        DeviceQueueDetails queueDetails = QueryQueueDetails(m_PhysicalDevice);

        VkCommandPoolCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        createInfo.queueFamilyIndex = queueDetails.GraphicsIndex;

        VkResult result = vkCreateCommandPool(m_Device, &createInfo, nullptr, &pool);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a command pool!");

        return pool;
    }

    b8 RendererBackend::CheckValidationLayerSupport(std::vector<const char *> &layers)
    {
        u32 count;
        vkEnumerateInstanceLayerProperties(&count, nullptr);
        std::vector<VkLayerProperties> available(count);
        vkEnumerateInstanceLayerProperties(&count, available.data());

        for (auto &layer : layers)
        {
            b8 found = false;
            for (u32 i = 0; i < count; i++)
            {
                if (strcmp(available[i].layerName, layer) == 0)
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

    b8 RendererBackend::CheckDeviceSuitable(const VkPhysicalDevice device)
    {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);
        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(device, &features);

        DeviceQueueDetails queueDetails = QueryQueueDetails(device);

        b8 isDiscrete = (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
        b8 hasFeatures = features.geometryShader;
        b8 hasExtensions = CheckDeviceExtensionSupport(device);

        b8 swapchainSupport = false;
        if (hasExtensions) {
            DeviceSwapchainSupportDetails swapchainDetails = QuerySwapchainSupportDetails(device);
            swapchainSupport = !swapchainDetails.PresentModes.empty() && !swapchainDetails.SurfaceFormats.empty();
        }

        return hasExtensions && queueDetails.IsComplete() && swapchainSupport;
    }

    b8 RendererBackend::CheckDeviceExtensionSupport(const VkPhysicalDevice device)
    {
        u32 count;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &count, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(count);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &count, availableExtensions.data());

        for (auto &name : m_DeviceExtensions)
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

    DeviceQueueDetails RendererBackend::QueryQueueDetails(const VkPhysicalDevice device)
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
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);
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

    DeviceSwapchainSupportDetails RendererBackend::QuerySwapchainSupportDetails(const VkPhysicalDevice device)
    {
        DeviceSwapchainSupportDetails swapchainDetails;

        u32 formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);
        if (formatCount != 0)
        {
            swapchainDetails.SurfaceFormats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, swapchainDetails.SurfaceFormats.data());
        }

        u32 presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, nullptr);
        swapchainDetails.PresentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, swapchainDetails.PresentModes.data());

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &swapchainDetails.Capabilities);

        return swapchainDetails;
    }
}