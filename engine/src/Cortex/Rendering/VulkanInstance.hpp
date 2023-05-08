#pragma once

#include "Cortex/Core/Window.hpp"

#include "vulkan/vulkan.h"
#include <vector>

namespace Cortex
{
    class VulkanInstance
    {
    public:
        VulkanInstance(const std::unique_ptr<Window> &window);
        ~VulkanInstance();

        inline VkInstance GetInstance() { return m_Instance; }
        inline VkSurfaceKHR GetSurface() { return m_Surface; }

    private:
        VkInstance CreateInstance();
        VkSurfaceKHR CreateSurface(const std::unique_ptr<Window> &window);
        VkDebugUtilsMessengerEXT CreateDebugMessenger();

        b8 CheckValidationLayerSupport(std::vector<const char *> &layers);

        VkInstance m_Instance;
        VkSurfaceKHR m_Surface;
        VkDebugUtilsMessengerEXT m_DebugMessenger;
    };
}