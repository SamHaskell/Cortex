#include "Badger/Graphics/VulkanBuffers.hpp"

namespace Badger {

    VulkanVertexBuffer vulkan_create_vertex_buffer(VkPhysicalDevice physicalDevice, VkDevice device, const std::vector<VulkanVertex>& vertices) {
        VulkanVertexBuffer vertexBuffer = {};
        vertexBuffer.VertexCount = static_cast<u32>(vertices.size());
        ASSERT(vertexBuffer.VertexCount > 2, "Vertex Count must be at least 3!");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexBuffer.VertexCount;
        vulkan_create_buffer(
            physicalDevice,
            device,
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            vertexBuffer.VertexBuffer,
            vertexBuffer.VertexBufferMemory
        );
        void* data;
        vkMapMemory(device, vertexBuffer.VertexBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), static_cast<u32>(bufferSize));
        vkUnmapMemory(device, vertexBuffer.VertexBufferMemory);
        return vertexBuffer;
    }

    void vulkan_destroy_vertex_buffer(VkDevice device, const VulkanVertexBuffer& vertexBuffer) {
        vkDestroyBuffer(device, vertexBuffer.VertexBuffer, nullptr);
        vkFreeMemory(device, vertexBuffer.VertexBufferMemory, nullptr);
    }
}