#pragma once

#include "Cortex/Utils/Asserts.h"
#include "Cortex/Graphics/Window.hpp"
#include "Cortex/Graphics/RenderBackend.hpp"

#include <algorithm>

namespace Cortex
{
    constexpr u32 unsetIndex {std::numeric_limits<u32>::max()};

    struct QueueInfo {
        u32 GraphicsIndex = unsetIndex;
        u32 PresentIndex = unsetIndex;
        b8 IsPresentQueueDiscrete = {GraphicsIndex != PresentIndex};
        b8 IsComplete = { GraphicsIndex == unsetIndex && PresentIndex == unsetIndex };
    };

    struct SwapchainSupportDetails
    {
        VkSurfaceCapabilitiesKHR Capabilities;
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR> PresentModes;
    };

    class RenderDevice {
        public:
        RenderDevice(const std::unique_ptr<RenderBackend>& backend, const std::unique_ptr<Window>& window);
        ~RenderDevice();

        private:
            inline void CreateSurface(const std::unique_ptr<Window>& window) { return window->CreateVulkanSurface(m_InstanceHandle, m_Surface); }
            void ChoosePhysicalDevice();
            void CreateDevice();
            void CreateCommandPool();

            const VkInstance m_InstanceHandle;
            VkPhysicalDevice m_PhysicalDevice;
            VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
            VkDevice m_Device;
            VkCommandPool m_CommandPool;

            VkQueue m_GraphicsQueue;
            VkQueue m_PresentQueue;

            QueueInfo m_QueueInfo;

            const std::vector<const char* > m_ValidationLayers = {"VK_LAYER_KHRONOS_validation"};
            const std::vector<const char* > m_Extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_portability_subset"};
            VkPhysicalDeviceFeatures m_DeviceFeatures = {};

            b8 IsDeviceSuitable(const VkPhysicalDevice device, const VkSurfaceKHR surface);
            QueueInfo GetQueueInfo(const VkPhysicalDevice device, const VkSurfaceKHR surface);
            b8 CheckDeviceExtensionSupport(const VkPhysicalDevice device);
            SwapchainSupportDetails QuerySwapchainSupport(const VkPhysicalDevice device);
            VkDeviceQueueCreateInfo FillQueueCreateInfo(uint32_t count, uint32_t queueFamilyIndex, float priority);
    };
}