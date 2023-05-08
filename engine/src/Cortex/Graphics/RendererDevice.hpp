#pragma once

#include "Cortex/Utils/Logging.hpp"
#include "vulkan/vulkan.h"
#include <vector>

namespace Cortex
{
    constexpr u32 UnsetIndex{std::numeric_limits<u32>::max()};

    struct RendererDeviceConfig
    {
        std::vector<const char *> DeviceExtensions;
        b8 RequireDiscreteGPU;
        b8 RequireComputeQueue;
        static RendererDeviceConfig Default();
    };

    struct DeviceQueueFamilies
    {
        u32 GraphicsFamily = UnsetIndex;
        u32 PresentFamily = UnsetIndex;
        u32 ComputeFamily = UnsetIndex;
        b8 IsComplete(b8 computeRequired);
    };

    struct DeviceSwapchainSupportDetails
    {
        std::vector<VkSurfaceFormatKHR> SurfaceFormats;
        std::vector<VkPresentModeKHR> PresentModes;
        VkSurfaceCapabilitiesKHR Capabilities;
    };

    class RendererDevice
    {
    public:
        RendererDevice(const RendererDeviceConfig &config, const VkInstance &instance, const VkSurfaceKHR &surface);
        ~RendererDevice();

        inline const VkDevice &GetDevice() { return m_Device; }
        inline const DeviceQueueFamilies &GetQueueFamilies() { return m_QueueFamilies; }
        inline const DeviceSwapchainSupportDetails &GetSwapchainSupportDetails() { return m_SwapchainSupportDetails; }
        inline const VkQueue GetGraphicsQueue() { return m_GraphicsQueue; }

        RendererDevice(const RendererDevice &) = delete;
        RendererDevice &operator=(const RendererDevice &) = delete;

    private:
        VkPhysicalDevice m_PhysicalDevice;
        DeviceQueueFamilies m_QueueFamilies;
        DeviceSwapchainSupportDetails m_SwapchainSupportDetails;
        VkDevice m_Device;
        VkQueue m_GraphicsQueue;
        VkQueue m_PresentQueue;
        VkQueue m_ComputeQueue;

        VkPhysicalDevice ChoosePhysicalDevice(const RendererDeviceConfig &config, const VkInstance &instance, const VkSurfaceKHR &surface);
        VkDevice CreateDeviceAndQueues(const RendererDeviceConfig &config);

        b8 CheckDeviceSuitable(const VkPhysicalDevice device, const VkSurfaceKHR surface, const RendererDeviceConfig &config);
        b8 CheckDeviceExtensionSupport(const VkPhysicalDevice device, const std::vector<const char *> requiredExtensions);
        DeviceQueueFamilies QueryQueueFamilies(const VkPhysicalDevice device, const VkSurfaceKHR surface, b8 requireCompute);
        DeviceSwapchainSupportDetails QuerySwapchainSupportDetails(const VkPhysicalDevice device, const VkSurfaceKHR surface);
        void LogPhysicalDeviceDetails();
    };

}