#pragma once

#include "Badger/Graphics/VulkanHelpers.hpp"
#include "Badger/Graphics/VulkanTypes.hpp"

#include "Badger/Core/Window.hpp"

#include "Badger/Graphics/GraphicsDevice.hpp"
#include "Badger/Graphics/RenderPass.hpp"
#include "Badger/Graphics/Swapchain.hpp"
#include "Badger/Graphics/Pipeline.hpp"

namespace Badger {
    class GraphicsContext {
        public:
            static std::unique_ptr<GraphicsContext> Create(const std::unique_ptr<Window>& window);
            GraphicsContext(const std::unique_ptr<Window>& window);
            ~GraphicsContext();
            GraphicsContext(const GraphicsContext&) = delete;
            GraphicsContext &operator=(const GraphicsContext&) = delete;

            inline std::shared_ptr<GraphicsDevice> GetDevice() { return m_GraphicsDevice; }
            inline RenderPass GetRenderPass() { return m_RenderPass; }
            VkCommandBuffer BeginFrame();
            bool BeginRenderPass(VkCommandBuffer commandBuffer);
            bool EndRenderPass(VkCommandBuffer commandBuffer);
            bool EndFrame();
            bool RecreateSwapchain(i32 width, i32 height);
            
        private:
            u32 m_MaxFrameIndex;
            u32 m_CurrentFrameIndex;
            VulkanSessionConfig m_Config;
            std::shared_ptr<GraphicsDevice> m_GraphicsDevice;
            VulkanSwapchainSpecification m_SwapchainSpec;
            RenderPass m_RenderPass;
            std::unique_ptr<Swapchain> m_Swapchain;
            VulkanDepthResources m_VulkanDepthResources;
            std::vector<VulkanFrameResources> m_FrameResources;
    };
}