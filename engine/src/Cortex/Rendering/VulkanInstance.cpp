#include "Cortex/Rendering/VulkanInstance.hpp"

#include "Cortex/Utils/Asserts.h"
#include "GLFW/glfw3.h"

namespace Cortex
{
    VulkanInstance::VulkanInstance(const std::unique_ptr<Window> &window)
        : m_Instance(CreateInstance()),
          m_Surface(CreateSurface(window)),
          m_DebugMessenger(CreateDebugMessenger())
    {
    }

    VulkanInstance::~VulkanInstance()
    {
        vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
        vkDestroyInstance(m_Instance, nullptr);
    }

    VkInstance VulkanInstance::CreateInstance()
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

    VkSurfaceKHR VulkanInstance::CreateSurface(const std::unique_ptr<Window> &window)
    {
        VkSurfaceKHR surface;
        window->CreateVulkanSurface(m_Instance, surface);
        return surface;
    }

    VkDebugUtilsMessengerEXT VulkanInstance::CreateDebugMessenger()
    {
            //TODO: Implement debug callback etc etc
    }

    b8 VulkanInstance::CheckValidationLayerSupport(std::vector<const char *> &layers)
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
}