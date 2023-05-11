#pragma once

#include "Cortex/Rendering/VulkanUtils.h"
#include "Cortex/Utils/Asserts.h"

#include "Cortex/Rendering/RenderInstance.hpp"

#include <deque>
#include <functional>

namespace Cortex
{
    constexpr u32 UnsetIndex{std::numeric_limits<u32>::max()};

    struct RenderDeviceConfig
    {
        std::vector<const char *> DeviceExtensions;
        b8 RequireDiscreteGPU;
        b8 RequireComputeQueue;
        static RenderDeviceConfig Default();
    };

    struct DeviceQueueFamilies
    {
        u32 GraphicsFamily = UnsetIndex;
        u32 PresentFamily = UnsetIndex;
        u32 ComputeFamily = UnsetIndex;
        u32 TransferFamily = UnsetIndex;
        b8 IsComplete(b8 computeRequired);
    };

    struct DeviceSwapchainSupportDetails
    {
        std::vector<VkSurfaceFormatKHR> SurfaceFormats;
        std::vector<VkPresentModeKHR> PresentModes;
        VkSurfaceCapabilitiesKHR Capabilities;
    };

    class RenderDevice
    {
    public:
        RenderDevice(const RenderDeviceConfig &config, const std::unique_ptr<RenderInstance> &instance);
        ~RenderDevice();

        inline const VkDevice& GetDevice() { return m_Device; }
        inline const DeviceQueueFamilies GetQueueFamilies() { return m_QueueFamilies; }
        inline const VkQueue GetGraphicsQueue() { return m_GraphicsQueue; }
        inline const VkQueue GetPresentQueue() { return m_PresentQueue; }
        inline const DeviceSwapchainSupportDetails GetSwapchainSupportDetails() { return m_SwapchainSupportDetails; }

        RenderDevice(const RenderDevice &) = delete;
        RenderDevice &operator=(const RenderDevice &) = delete;

    private:
        VkPhysicalDevice m_PhysicalDevice;
        DeviceQueueFamilies m_QueueFamilies;
        DeviceSwapchainSupportDetails m_SwapchainSupportDetails;
        VkDevice m_Device;

        VkQueue m_GraphicsQueue;
        VkQueue m_PresentQueue;
        VkQueue m_ComputeQueue;
        VkQueue m_TransferQueue;

        VkPhysicalDevice ChoosePhysicalDevice(const RenderDeviceConfig &config, const VkInstance &instance, const VkSurfaceKHR &surface);
        VkDevice CreateDeviceAndQueues(const RenderDeviceConfig &config);

        b8 CheckDeviceSuitable(const RenderDeviceConfig &config, const VkPhysicalDevice device, const VkSurfaceKHR surface);

        b8 CheckDeviceExtensionSupport(const VkPhysicalDevice device, const std::vector<const char *> requiredExtensions);
        DeviceQueueFamilies QueryQueueFamilies(const VkPhysicalDevice device, const VkSurfaceKHR surface, b8 requireCompute);
        DeviceSwapchainSupportDetails QuerySwapchainSupportDetails(const VkPhysicalDevice device, const VkSurfaceKHR surface);

        void LogPhysicalDeviceDetails();
    };
}