#include "Badger/Graphics/Renderer.hpp"

namespace Badger {
    std::unique_ptr<Renderer> Renderer::Create(const std::unique_ptr<GraphicsContext>& context) {
        return std::make_unique<Renderer>(context);
    }

    Renderer::Renderer(const std::unique_ptr<GraphicsContext>& context) {
        m_GraphicsDevice = context->GetDevice();
        m_PipelineLayout = vulkan_create_pipeline_layout(m_GraphicsDevice->Device);
        
        auto pipelineConfig = VulkanPipelineConfig::Default();
        pipelineConfig.RenderPass = context->GetRenderPass().Pass;
        pipelineConfig.PipelineLayout = m_PipelineLayout;
        m_Pipeline = Pipeline::Create(m_GraphicsDevice, "../../testbed/assets/shaders/basic.vert.spv", "../../testbed/assets/shaders/basic.frag.spv", pipelineConfig);

        m_Texture = vulkan_create_texture_2D(m_GraphicsDevice, "../../testbed/assets/textures/tex_DebugGrid.png");
    }

    Renderer::~Renderer() {
        vkDeviceWaitIdle(m_GraphicsDevice->Device);
        vkDestroyPipelineLayout(m_GraphicsDevice->Device, m_PipelineLayout, nullptr);
    }

    void Renderer::DrawScene(VkCommandBuffer commandBuffer, const Scene& scene) {

        m_Pipeline->Bind(commandBuffer);
        for (auto& e : scene.Entities) {
            VulkanPushData push;
            push.Transform = scene.MainCamera.ProjectionMatrix * scene.MainCamera.ViewMatrix * e.Transform.ModelToWorld;
            vkCmdPushConstants(commandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(VulkanPushData), &push);
            e.Mesh.Model->Bind(commandBuffer);
            e.Mesh.Model->Draw(commandBuffer);
        }
    }
}