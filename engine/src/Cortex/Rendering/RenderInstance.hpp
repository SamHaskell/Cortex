#pragma once

#include "Cortex/Rendering/VulkanUtils.h"
#include "Cortex/Utils/Asserts.h"

#include "Cortex/Core/Window.hpp"

namespace Cortex
{
    struct RenderInstanceConfig
    {
        std::vector<const char *> InstanceExtensions;
        std::vector<const char *> ValidationLayers;
        static RenderInstanceConfig Default();
    };

    class RenderInstance
    {
    public:
        RenderInstance(const RenderInstanceConfig &config, const std::unique_ptr<Window> &window);
        ~RenderInstance();

        inline const VkInstance GetInstance() { return m_Instance; }
        inline const VkSurfaceKHR GetSurface() { return m_Surface; }

        RenderInstance(const RenderInstance &) = delete;
        RenderInstance &operator=(const RenderInstance &) = delete;

    private:
        RenderInstanceConfig m_CurrentConfig;
        VkInstance m_Instance;
        VkSurfaceKHR m_Surface;

        VkInstance CreateInstance(const RenderInstanceConfig &config);
        VkSurfaceKHR CreateSurface(const std::unique_ptr<Window> &window);

        b8 CheckValidationLayerSupport(const std::vector<const char *> &layers);
    };
}