#pragma once

#include "Cortex/Utils/Logging.hpp"
#include "Cortex/Core/Window.hpp"
#include "vulkan/vulkan.h"
#include <vector>

namespace Cortex
{
    struct RendererInstanceConfig
    {
        std::vector<const char *> InstanceExtensions;
        std::vector<const char *> ValidationLayers;
        static RendererInstanceConfig Default();
    };

    class RendererInstance
    {
    public:
        RendererInstance(const RendererInstanceConfig& config, const Window& window);
        ~RendererInstance();

        inline const VkInstance GetInstance() { return m_Instance; }
        inline const VkSurfaceKHR GetSurface() { return m_Surface; }

        RendererInstance(const RendererInstance &) = delete;
        RendererInstance &operator=(const RendererInstance &) = delete;

    private:
        VkInstance m_Instance;
        VkSurfaceKHR m_Surface;
        VkDebugUtilsMessengerEXT m_DebugMessenger;

        VkInstance CreateInstance(const RendererInstanceConfig& config);
        VkSurfaceKHR CreateSurface(const Window& window);
        VkDebugUtilsMessengerEXT CreateDebugMessenger(const RendererInstanceConfig &config);

        b8 CheckValidationLayerSupport(const std::vector<const char *> &layers);
    };    

}