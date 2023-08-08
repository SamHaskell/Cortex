#pragma once

#include "Badger/Graphics/VulkanHelpers.hpp"
#include "Badger/Graphics/VulkanTypes.hpp"

#include "Badger/Core/Window.hpp"

#include "Badger/Graphics/GraphicsDevice.hpp"
#include "Badger/Graphics/RenderPass.hpp"
#include "Badger/Graphics/Swapchain.hpp"
#include "Badger/Graphics/Pipeline.hpp"
#include "Badger/Graphics/Model.hpp"

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

            bool BeginFrame(VkCommandBuffer& commandBuffer);
            bool BeginRenderPass(VkCommandBuffer commandBuffer);
            bool EndRenderPass(VkCommandBuffer commandBuffer);
            bool EndFrame();

            bool OnFramebufferResize(i32 width, i32 height);
            bool RecreateSwapchain();

            inline std::shared_ptr<Model> LoadModel(const std::vector<VulkanVertex>& vertices, const std::vector<VulkanIndex>& indices) {
                return std::make_shared<Model>(m_GraphicsDevice, vertices, indices);
            }

            std::shared_ptr<Model> LoadModelFromOBJ(const std::string& path);
            
        private:
            u32 m_MaxFrameIndex;
            u32 m_CurrentFrameIndex;
            VulkanSessionConfig m_Config;
            std::shared_ptr<GraphicsDevice> m_GraphicsDevice;
            VulkanSwapchainSpecification m_SwapchainSpec;
            RenderPass m_RenderPass;
            bool m_SwapchainSuboptimal;
            std::unique_ptr<Swapchain> m_Swapchain;
            VulkanDepthResources m_VulkanDepthResources;
            std::vector<VulkanFrameResources> m_FrameResources;
    };
}