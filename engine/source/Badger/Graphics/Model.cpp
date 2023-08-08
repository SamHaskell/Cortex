#include "Badger/Graphics/Model.hpp"

namespace Badger {
    Model::Model(std::shared_ptr<GraphicsDevice> device, const std::vector<VulkanVertex>& vertices, const std::vector<VulkanIndex>& indices) {
        m_GraphicsDevice = device;
        m_VertexBuffer = vulkan_create_vertex_buffer(m_GraphicsDevice, vertices);
        m_IndexBuffer = vulkan_create_index_buffer(m_GraphicsDevice, indices);

        LOG_INFO("Vertices: %i. Indices: %i.", m_VertexBuffer.VertexCount, m_IndexBuffer.IndexCount);
    }

    Model::~Model() {
        vulkan_destroy_vertex_buffer(m_GraphicsDevice, m_VertexBuffer);
        vulkan_destroy_index_buffer(m_GraphicsDevice, m_IndexBuffer);
    }

    void Model::Bind(VkCommandBuffer commandBuffer) {
        VkBuffer buffers[] = { m_VertexBuffer.VertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer.IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
    }

    void Model::Draw(VkCommandBuffer commandBuffer) {
        vkCmdDrawIndexed(commandBuffer, m_IndexBuffer.IndexCount, 1, 0, 0, 0);
    }
}