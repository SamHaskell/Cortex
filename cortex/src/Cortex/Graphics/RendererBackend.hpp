#pragma once

#include "Cortex/Utils/Asserts.h"

#include "vulkan/vulkan.h"

#include <memory>
#include <vector>

namespace Cortex
{

    class Window;
    constexpr u32 UnsetIndex{std::numeric_limits<u32>::max()};

    struct DeviceQueueDetails
    {
        u32 GraphicsIndex = UnsetIndex;
        u32 PresentIndex = UnsetIndex;
        inline b8 IsComplete() { return GraphicsIndex != UnsetIndex && PresentIndex != UnsetIndex; }
    };

    struct DeviceSwapchainSupportDetails
    {
        std::vector<VkSurfaceFormatKHR> SurfaceFormats;
        std::vector<VkPresentModeKHR> PresentModes;
        VkSurfaceCapabilitiesKHR Capabilities;
    };

    class RendererBackend
    {
    public:
        RendererBackend(const std::unique_ptr<Window> &window);
        ~RendererBackend();

        inline const VkSurfaceKHR& GetSurface() { return m_Surface; }
        inline const VkDevice& GetDevice() { return m_Device; }
        inline const VkCommandPool& GetCommandPool() { return m_CommandPool; }
        inline const VkQueue& GetGraphicsQueue() { return m_GraphicsQueue; }
        inline const VkQueue& GetPresentQueue() { return m_PresentQueue; }

        inline const DeviceQueueDetails GetQueueDetails() { return QueryQueueDetails(m_PhysicalDevice); }
        inline const DeviceSwapchainSupportDetails GetSwapchainSupportDetails() { return QuerySwapchainSupportDetails(m_PhysicalDevice); }

    private:
        VkInstance CreateInstance();
        VkSurfaceKHR CreateSurface(Window &window);
        VkPhysicalDevice ChoosePhysicalDevice();
        VkDevice CreateDevice();
        VkCommandPool CreateCommandPool();

        b8 CheckValidationLayerSupport(std::vector<const char *> &layers);
        b8 CheckDeviceSuitable(const VkPhysicalDevice device);
        b8 CheckDeviceExtensionSupport(const VkPhysicalDevice device);

        DeviceQueueDetails QueryQueueDetails(const VkPhysicalDevice device);
        DeviceSwapchainSupportDetails QuerySwapchainSupportDetails(const VkPhysicalDevice device);

    private:
        const std::vector<const char *> m_DeviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            "VK_KHR_portability_subset"};

        VkInstance m_Instance;
        VkSurfaceKHR m_Surface;
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice m_Device;
        VkCommandPool m_CommandPool;

        VkQueue m_GraphicsQueue;
        VkQueue m_PresentQueue;
    };
}