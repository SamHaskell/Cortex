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

            bool BeginFrame();
            bool EndFrame();
            bool RecreateSwapchain(i32 width, i32 height);
        private:
            u32 m_MaxFrameIndex;
            u32 m_CurrentFrameIndex;
            VulkanSessionConfig m_Config;
            GraphicsDevice m_GraphicsDevice;
            VulkanSwapchainSpecification m_SwapchainSpec;
            RenderPass m_RenderPass;
            std::unique_ptr<Swapchain> m_Swapchain;
            VkPipelineLayout m_TestPipelineLayout;
            std::shared_ptr<Pipeline> m_TestPipeline;
            VulkanDepthResources m_VulkanDepthResources;
            std::vector<VulkanFrameResources> m_FrameResources;
    };
}