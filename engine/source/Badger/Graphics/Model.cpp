#include "Badger/Graphics/Model.hpp"

namespace Badger {
    Model::Model(std::shared_ptr<GraphicsDevice> device, const std::vector<VulkanVertex>& vertices) {
        m_GraphicsDevice = device;
        m_VertexBuffer = vulkan_create_vertex_buffer(m_GraphicsDevice->PhysicalDevice, m_GraphicsDevice->Device, vertices);
    }

    Model::~Model() {
        vulkan_destroy_vertex_buffer(m_GraphicsDevice->Device, m_VertexBuffer);
    }

    void Model::Bind(VkCommandBuffer commandBuffer) {
        VkBuffer buffers[] = { m_VertexBuffer.VertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    }

    void Model::Draw(VkCommandBuffer commandBuffer) {
        vkCmdDraw(commandBuffer, m_VertexBuffer.VertexCount, 1, 0, 0);
    }
}