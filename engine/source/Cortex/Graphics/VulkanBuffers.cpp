#include "Cortex/Graphics/VulkanBuffers.hpp"

namespace Cortex {

    void vulkan_copy_buffer(const std::shared_ptr<GraphicsDevice> device, VkBuffer src, VkBuffer dst, VkDeviceSize size) {
        VkCommandBuffer commandBuffer = vulkan_begin_transient_commands(device->Device, device->TransferCommandPool);
        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyRegion);
        vulkan_end_transient_commands(device->Device, device->TransferCommandPool, device->Queues.Transfer, commandBuffer);
    }

    VulkanVertexBuffer vulkan_create_vertex_buffer(const std::shared_ptr<GraphicsDevice> device, const std::vector<VulkanVertex>& vertices) {
        VulkanVertexBuffer vertexBuffer = {};
        vertexBuffer.VertexCount = static_cast<u32>(vertices.size());
        ASSERT(vertexBuffer.VertexCount > 2, "Vertex Count must be at least 3!");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexBuffer.VertexCount;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingMemory;

        vulkan_create_buffer(
            device->PhysicalDevice,
            device->Device,
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingMemory
        );

        void* data;
        vkMapMemory(device->Device, stagingMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), static_cast<u32>(bufferSize));
        vkUnmapMemory(device->Device, stagingMemory);

        vulkan_create_buffer(
            device->PhysicalDevice,
            device->Device,
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            vertexBuffer.VertexBuffer,
            vertexBuffer.VertexBufferMemory
        );

        vulkan_copy_buffer(
            device,
            stagingBuffer,
            vertexBuffer.VertexBuffer,
            bufferSize
        );

        vkDestroyBuffer(device->Device, stagingBuffer, nullptr);
        vkFreeMemory(device->Device, stagingMemory, nullptr);
        
        return vertexBuffer;
    }

    void vulkan_destroy_vertex_buffer(const std::shared_ptr<GraphicsDevice> device, const VulkanVertexBuffer& vertexBuffer) {
        vkDestroyBuffer(device->Device, vertexBuffer.VertexBuffer, nullptr);
        vkFreeMemory(device->Device, vertexBuffer.VertexBufferMemory, nullptr);
    }

    VulkanIndexBuffer vulkan_create_index_buffer(const std::shared_ptr<GraphicsDevice> device, const std::vector<VulkanIndex>& indices) {
        VulkanIndexBuffer indexBuffer = {};
        indexBuffer.IndexCount = static_cast<u32>(indices.size());
        ASSERT(indexBuffer.IndexCount > 2, "Index Count must be at least 3!");
        VkDeviceSize bufferSize = sizeof(indices[0]) * indexBuffer.IndexCount;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingMemory;

        vulkan_create_buffer(
            device->PhysicalDevice,
            device->Device,
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingMemory
        );

        void* data;
        vkMapMemory(device->Device, stagingMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), static_cast<u32>(bufferSize));
        vkUnmapMemory(device->Device, stagingMemory);

        vulkan_create_buffer(
            device->PhysicalDevice,
            device->Device,
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            indexBuffer.IndexBuffer,
            indexBuffer.IndexBufferMemory
        );

        vulkan_copy_buffer(
            device,
            stagingBuffer,
            indexBuffer.IndexBuffer,
            bufferSize
        );

        vkDestroyBuffer(device->Device, stagingBuffer, nullptr);
        vkFreeMemory(device->Device, stagingMemory, nullptr);

        return indexBuffer;
    }

    void vulkan_destroy_index_buffer(const std::shared_ptr<GraphicsDevice> device, const VulkanIndexBuffer& indexBuffer) {
        vkDestroyBuffer(device->Device, indexBuffer.IndexBuffer, nullptr);
        vkFreeMemory(device->Device, indexBuffer.IndexBufferMemory, nullptr);
    }

    std::vector<VulkanUniformBuffer> vulkan_create_uniform_buffers(const std::shared_ptr<GraphicsDevice> device, u32 count) {
        std::vector<VulkanUniformBuffer> buffers(count);
        VkDeviceSize bufferSize = sizeof(VulkanCameraUniformData);
        for (u32 i = 0; i < count; i++) {
            vulkan_create_buffer(
                device->PhysicalDevice, 
                device->Device,
                bufferSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                buffers[i].UniformBuffer,
                buffers[i].UniformBufferMemory
            );
            vkMapMemory(device->Device, buffers[i].UniformBufferMemory, 0, bufferSize, 0, &buffers[i].UniformBufferMapped);
        }
        return buffers;
    }
}