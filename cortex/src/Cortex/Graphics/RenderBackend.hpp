#pragma once

#include "Cortex/Utils/Asserts.h"
#include "Cortex/Graphics/Window.hpp"

#include "vulkan/vulkan.h"

#include <vector>

namespace Cortex
{
    class RenderBackend
    {
    public:
        RenderBackend(b8 enableValidation);
        ~RenderBackend();
        
        void LogInstanceInformation(LogLevel logLevel);

        inline const VkInstance GetInstance() { return m_Instance; }
        inline const VkDebugUtilsMessengerEXT GetDebugger() { return m_Debugger; }

    private:
        VkInstance m_Instance;
        VkDebugUtilsMessengerEXT m_Debugger;
        std::vector<const char *> m_ValidationLayers;
        std::vector<const char *> m_InstanceExtensions;
        std::vector<const char *> m_DeviceExtensions;
        b8 m_EnableValidationLayers;

        void CreateInstance();
        void SetupDebugger();

        b8 CheckInstanceExtensionSupport(std::vector<const char *> extensions);
        b8 CheckInstanceLayerSupport(std::vector<const char *> layers);

        VkResult CreateDebugUtilsMessengerEXT(
            VkInstance instance,
            const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
            const VkAllocationCallbacks *pAllocator,
            VkDebugUtilsMessengerEXT *pDebugMessenger);

        void DestroyDebugUtilsMessengerEXT(
            VkInstance instance,
            VkDebugUtilsMessengerEXT debugMessenger,
            const VkAllocationCallbacks *pAllocator);

        void FillDebugMessengerCreateInfo(
            VkDebugUtilsMessengerCreateInfoEXT &createInfo);
    };

}