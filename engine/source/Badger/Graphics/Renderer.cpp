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
    }

    Renderer::~Renderer() {
        vkDestroyPipelineLayout(m_GraphicsDevice->Device, m_PipelineLayout, nullptr);
    }

    void Renderer::DrawEntities(std::vector<Entity> entities) {
        if (entities.size() == 0) {
            LOG_WARN("Submitted request to draw no entities to Renderer.");
            return;
        }

        for (Entity e : entities) {
            LOG_DEBUG("Drawing Entity. ID: %u.", e.Identifier);
        }
    }
}