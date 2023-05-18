#include "Cortex/Rendering/RenderContext.hpp"

#include "Cortex/Rendering/Pipeline.hpp"
#include "Cortex/Rendering/Buffer.hpp"
#include "Cortex/Rendering/Vertex.hpp"

#include "Cortex/Rendering/StaticMesh.hpp"

#include <memory>

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
          m_Swapchain(std::make_unique<Swapchain>(contextConfig.SwapchainConfig, m_Instance, m_Device))
    {
        std::shared_ptr<Shader> testFrag = std::make_shared<Shader>(m_Device, "../engine/assets/Shaders/basic.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
        std::shared_ptr<Shader> testVert = std::make_shared<Shader>(m_Device, "../engine/assets/Shaders/basic.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
        m_TestPipeline = std::make_shared<Pipeline>(m_Device, m_Swapchain, testVert, testFrag);

        std::vector<Vertex> vertices = {
            {{-0.5f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
            {{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
            {{-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},

            {{0.5f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
            {{1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
            {{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
            
            {{0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}          
        };

        std::vector<uint16_t> indices = {
            0, 1, 2, 2, 3, 0};

        m_TestMesh = std::make_shared<StaticMesh>(m_Device, vertices, indices);
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

    void RenderContext::RecordTestCommandBuffer()
    {
        VkCommandBuffer commandBuffer = m_Swapchain->GetCurrentCommandBuffer();

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_TestPipeline->GetPipeline());

        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<f32>(m_Swapchain->GetSwapchainExtent().width);
        viewport.height = static_cast<f32>(m_Swapchain->GetSwapchainExtent().height);
        viewport.maxDepth = 1.0f;
        viewport.minDepth = 0.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor = {};
        scissor.offset = {0, 0};
        scissor.extent = m_Swapchain->GetSwapchainExtent();
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        //////////////////////////////////////
        // Draw Stuff Here ///////////////////
        //////////////////////////////////////

        DrawMesh(m_TestMesh, commandBuffer);
    }

    void RenderContext::DrawMesh(const std::shared_ptr<StaticMesh> &mesh, VkCommandBuffer commandBuffer)
    {
        VkBuffer vertexBuffers[] = {mesh->GetVertexBuffer()->GetBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdDraw(commandBuffer, mesh->GetVertexCount(), 1, 0, 0);
    }
}