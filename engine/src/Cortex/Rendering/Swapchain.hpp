#pragma once

#include "Cortex/Rendering/VulkanUtils.hpp"
#include "Cortex/Utils/Asserts.h"

#include "Cortex/Rendering/RenderInstance.hpp"
#include "Cortex/Rendering/RenderDevice.hpp"

namespace Cortex
{
    struct SwapchainConfig
    {
        u32 FramebufferWidth;
        u32 FramebufferHeight;
        static SwapchainConfig Default(u32 width, u32 height);
    };

    struct SyncObjects
    {
        VkSemaphore ImageAvailableSemaphore;
        VkSemaphore RenderFinishedSemaphore;
        VkFence InFlightFence;
    };

    class Swapchain
    {
        public:
        Swapchain(const SwapchainConfig& swapchainConfig, const std::unique_ptr<RenderInstance>& instance, const std::shared_ptr<RenderDevice>& device);
        ~Swapchain();

        inline const VkSwapchainKHR GetSwapchain() { return m_Swapchain; }
        inline const VkFormat GetSwapchainFormat() { return m_SwapchainFormat; }
        inline const VkExtent2D GetSwapchainExtent() { return m_SwapchainExtent; }
        inline const VkRenderPass GetRenderPass() { return m_RenderPass; }

        inline const u32 GetImageCount() { return m_ImageCount; }

        inline const VkCommandBuffer GetCurrentCommandBuffer() { return m_DrawCommandBuffers[m_CurrentFrameIndex]; }
        inline const VkFramebuffer GetCurrentFramebuffer() { return m_Framebuffers[m_CurrentImageIndex]; }

        inline void SetClearColor(VkClearColorValue color) { m_ClearColor = color; }

        void BeginFrame();
        void EndFrame(const VkQueue graphicsQueue);
        void PresentFrame(const VkQueue presentQueue);

        Swapchain(const Swapchain &) = delete;
        Swapchain &operator=(const Swapchain &) = delete;

        private:
        const VkDevice m_DeviceHandle;

        VkClearColorValue m_ClearColor = {0.08, 0.08, 0.1, 1.0};

        u32 m_CurrentFrameIndex;
        u32 m_CurrentImageIndex;
        u32 m_ImageCount;

        VkFormat m_SwapchainFormat;
        VkPresentModeKHR m_SwapchainPresentMode;
        VkExtent2D m_SwapchainExtent;

        VkSwapchainKHR m_Swapchain;
        VkRenderPass m_RenderPass;

        std::vector<VkImage> m_Images;
        std::vector<VkImageView> m_ImageViews;
        std::vector<VkFramebuffer> m_Framebuffers;

        VkCommandPool m_DrawCommandPool;
        std::vector<VkCommandBuffer> m_DrawCommandBuffers;
        std::vector<SyncObjects> m_DrawSyncObjects;

        void CreateSwapchain(const SwapchainConfig &config, const VkSurfaceKHR &surface, const DeviceQueueFamilies& queueDetails, const DeviceSwapchainSupportDetails& swapchainDetails);
        void CreateRenderPass();
        void RetrieveImages();
        void CreateImageViews();
        void CreateFramebuffers();
        void CreateDrawCommandPool(const DeviceQueueFamilies &queueDetails);
        void CreateDrawCommandBuffers();
        void CreateDrawSyncObjects();

        void DestroySwapchainObjects();

        VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR> surfaceFormats);
        VkPresentModeKHR ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR> presentModes);
        VkExtent2D ChooseSwapchainExtents(const VkSurfaceCapabilitiesKHR capabilities, u32 windowWidth, u32 windowHeight);
        void LogSwapchainDetails();
    };
}