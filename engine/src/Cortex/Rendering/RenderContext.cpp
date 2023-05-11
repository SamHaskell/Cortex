#include "Cortex/Rendering/RenderContext.hpp"

#include "Cortex/Rendering/Pipeline.hpp"

namespace Cortex
{
    RenderContextConfig RenderContextConfig::Default(u32 width, u32 height)
    {
        RenderContextConfig config;
        config.InstanceConfig = RenderInstanceConfig::Default();
        config.DeviceConfig = RenderDeviceConfig::Default();
        config.SwapchainConfig = SwapchainConfig::Default(width, height);
        return config;
    }

    RenderContext::RenderContext(const RenderContextConfig &contextConfig, const std::unique_ptr<Window> &window)
        : m_Instance(std::make_unique<RenderInstance>(contextConfig.InstanceConfig, window)),
          m_Device(std::make_unique<RenderDevice>(contextConfig.DeviceConfig, m_Instance)),
          m_Swapchain(std::make_unique<Swapchain>(contextConfig.SwapchainConfig, m_Instance, m_Device)),
          m_Pipeline(CreatePipeline())
    {
    }

    RenderContext::~RenderContext()
    {
        const VkDevice &device = m_Device->GetDevice();
        vkDeviceWaitIdle(device);
    }

    void RenderContext::BeginFrame()
    {
        m_Swapchain->BeginFrame();
    }

    void RenderContext::EndFrame()
    {
        m_Swapchain->EndFrame(m_Device->GetGraphicsQueue());
        m_Swapchain->PresentFrame(m_Device->GetPresentQueue());
    }

    VkPipeline RenderContext::CreatePipeline()
    {
        return Pipeline::CreatePipeline(m_Device->GetDevice(), "../engine/assets/Shaders/basic.vert.spv", "../engine/assets/Shaders/basic.frag.spv", PipelineConfig::Default(), m_Swapchain->GetRenderPass());
    }

    void RenderContext::RecordTestCommandBuffer()
    {
        VkCommandBuffer commandBuffer = m_Swapchain->GetCurrentCommandBuffer();


        // vkCmdBindPipeline(m_DrawCommandBuffers[m_CurrentFrameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);

        // VkViewport viewport = {};
        // viewport.x = 0.0f;
        // viewport.y = 0.0f;
        // viewport.width = static_cast<f32>(m_Swapchain->GetSwapchainExtent().width);
        // viewport.height = static_cast<f32>(m_Swapchain->GetSwapchainExtent().height);
        // viewport.maxDepth = 1.0f;
        // viewport.minDepth = 0.0f;
        // vkCmdSetViewport(m_DrawCommandBuffers[m_CurrentFrameIndex], 0, 1, &viewport);

        // VkRect2D scissor = {};
        // scissor.offset = {0, 0};
        // scissor.extent = m_Swapchain->GetSwapchainExtent();
        // vkCmdSetScissor(m_DrawCommandBuffers[m_CurrentFrameIndex], 0, 1, &scissor);

        // vkCmdDraw(m_DrawCommandBuffers[m_CurrentFrameIndex], 3, 1, 0, 0);


    }
}