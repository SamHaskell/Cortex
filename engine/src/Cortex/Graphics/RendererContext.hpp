#pragma once

#include "Cortex/Utils/Asserts.h"

#include "Cortex/Graphics/RendererBackend.hpp"

#include "glm/glm.hpp"

namespace Cortex
{
    struct Vertex {
        glm::vec2 pos;
        glm::vec3 color;

        static VkVertexInputBindingDescription GetBindingDescription();
        static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions();
    };

    class RendererContext
    {
    public:
        RendererContext(const std::unique_ptr<RendererBackend> &backend, u32 windowWidth, u32 windowHeight);
        ~RendererContext();

        b8 DrawTestFrame();

    private:
        b8 RenderBegin();
        b8 Render();
        b8 RenderEnd();

        VkSwapchainKHR CreateSwapchain(const std::unique_ptr<RendererBackend> &backend, u32 windowWidth, u32 windowHeight);
        std::vector<VkImage> RetrieveSwapchainImages();
        std::vector<VkImageView> CreateSwapchainImageViews();
        VkRenderPass CreateRenderPass();
        VkPipeline CreateGraphicsPipeline();
        VkBuffer CreateVertexBuffer();
        std::vector<VkFramebuffer> CreateFramebuffers();
        std::vector<VkCommandBuffer> CreateCommandBuffers(const std::unique_ptr<RendererBackend> &backend);
        void CreateSynchronisationObjects();

        VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR> surfaceFormats);
        VkPresentModeKHR ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR> presentModes);
        VkExtent2D ChooseSwapchainExtents(const VkSurfaceCapabilitiesKHR capabilities, u32 windowWidth, u32 windowHeight);
        VkShaderModule CreateShaderModule(const std::vector<char>& code);
        std::vector<char> ReadShader(const std::string& path);

    private:
        const int MAX_FRAMES_IN_FLIGHT = 2;

        const std::vector<Vertex> m_TestVertices = {
            {{ 0.0f, -0.5f}, {0.8f, 0.2f, 0.0f}},
            {{ 0.5f,  0.5f}, {0.0f, 0.8f, 0.2f}},
            {{-0.5f,  0.5f}, {0.2f, 0.0f, 0.8f}}
        };

        const VkDevice& m_DeviceHandle;
        const VkQueue& m_GraphicsQueueHandle;
        const VkQueue& m_PresentQueueHandle;

        VkSwapchainKHR m_Swapchain;
        std::vector<VkImage> m_SwapchainImages;
        std::vector<VkImageView> m_SwapchainImageViews;
        VkFormat m_SwapchainImageFormat;
        VkExtent2D m_SwapchainImageExtent;
        
        VkRenderPass m_RenderPass;
        VkPipeline m_GraphicsPipeline;
        VkPipelineLayout m_PipelineLayout;

        VkBuffer m_TestBuffer;

        std::vector<VkFramebuffer> m_Framebuffers;
        std::vector<VkCommandBuffer> m_CommandBuffers;
        std::vector<VkSemaphore> m_ImageAvailableSemaphores;
        std::vector<VkSemaphore> m_RenderFinishedSemaphores;
        std::vector<VkFence> m_FrameFlightFences;

        u32 m_CurrentImageIndex = 0;
        u32 m_CurrentFrame = 0;
    };
}