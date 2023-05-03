#include "Cortex/Graphics/RenderBackend.hpp"

#include "Cortex/Utils/Logging.hpp"

#include <sstream>
#include <iostream>

namespace Cortex
{
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData)
    {
        CX_ERROR("%s", pCallbackData->pMessage);

        return VK_FALSE;
    }

    RenderBackend::RenderBackend(b8 enableValidation)
    {
        m_EnableValidationLayers = enableValidation;
        CreateInstance();
        SetupDebugger();
    }

    RenderBackend::~RenderBackend() {
        DestroyDebugUtilsMessengerEXT(m_Instance, m_Debugger, nullptr);
        vkDestroyInstance(m_Instance, nullptr);
    }

    void RenderBackend::LogInstanceInformation(LogLevel logLevel)
    {
        // TODO: Implement, to allow debugging.
    }

    void RenderBackend::CreateInstance()
    {
        // APP INFO
        VkApplicationInfo appInfo = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
        appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
        appInfo.pApplicationName = "Vulkan App";
        appInfo.engineVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
        appInfo.pEngineName = "Vulkan Renderer";
        appInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 0);

        // GET REQUIRED EXTENSIONS
        u32 count;
        m_InstanceExtensions.clear();
        const char **glfwRequiredExtensions = glfwGetRequiredInstanceExtensions(&count);
        for (u32 i = 0; i < count; i++)
        {
            m_InstanceExtensions.push_back(glfwRequiredExtensions[i]);
        }
        m_InstanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME); // For MoltenVK.
        m_InstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);             // For the Debug Messenger.

        // CHECK EXTENSION SUPPORT
        bool supported = CheckInstanceExtensionSupport(m_InstanceExtensions);
        CX_ASSERT_MSG(supported, "This implementation of Vulkan does not support the specified Instance Extensions");

        // CHOOSE VALIDATION LAYERS
        m_ValidationLayers.clear();
        m_ValidationLayers.push_back("VK_LAYER_KHRONOS_validation");

        // CHECK VALIDATION LAYER SUPPORT
        supported = CheckInstanceLayerSupport(m_ValidationLayers);
        CX_ASSERT_MSG(supported, "This implementation of Vulkan does not support the specified Validation Layers");
        

        VkInstanceCreateInfo createInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
        createInfo.pApplicationInfo = &appInfo;
        createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
        createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(m_InstanceExtensions.size());
        createInfo.ppEnabledExtensionNames = m_InstanceExtensions.data();

        VkResult result = vkCreateInstance(&createInfo, nullptr, &m_Instance);
        CX_ASSERT_MSG((result == VK_SUCCESS), "Failed to create a Vulkan Instance");
    }

    void RenderBackend::SetupDebugger()
    {
        if (!m_EnableValidationLayers)
            return;
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        FillDebugMessengerCreateInfo(createInfo);
        VkResult result = CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_Debugger);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a Vulkan Debug Messenger");
    }

    b8 RenderBackend::CheckInstanceExtensionSupport(std::vector<const char *> extensions)
    {
        uint32_t availableExtensionCount;
        vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions.data());

        for (auto &name : extensions)
        {
            b8 found = false;
            for (size_t i = 0; i < availableExtensions.size(); i++)
            {
                if (strcmp(name, availableExtensions[i].extensionName) == 0)
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

    b8 RenderBackend::CheckInstanceLayerSupport(std::vector<const char *> layers)
    {
        u32 availableLayerCount;
        vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(availableLayerCount);
        vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data());

        for (auto &name : layers)
        {
            bool found = false;
            for (size_t i = 0; i < availableLayers.size(); i++)
            {
                if (strcmp(name, availableLayers[i].layerName) == 0)
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

    VkResult RenderBackend::CreateDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
        const VkAllocationCallbacks *pAllocator,
        VkDebugUtilsMessengerEXT *pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            instance,
            "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void RenderBackend::DestroyDebugUtilsMessengerEXT(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks *pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            instance,
            "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            func(instance, debugMessenger, pAllocator);
        }
    }

    void RenderBackend::FillDebugMessengerCreateInfo(
        VkDebugUtilsMessengerCreateInfoEXT &createInfo)
    {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = DebugCallback;
        createInfo.pUserData = nullptr; // Optional
    }
}