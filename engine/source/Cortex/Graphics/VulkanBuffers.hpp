#pragma once

#include "Cortex/Graphics/VulkanHelpers.hpp"
#include "Cortex/Graphics/VulkanTypes.hpp"

#include "Cortex/Graphics/GraphicsDevice.hpp"

namespace Cortex {
    struct VulkanVertexBuffer {
        VkBuffer VertexBuffer;
        VkDeviceMemory VertexBufferMemory;
        u32 VertexCount;
    };

    struct VulkanIndexBuffer {
        VkBuffer IndexBuffer;
        VkDeviceMemory IndexBufferMemory;
        u32 IndexCount;
    };

    struct VulkanUniformBuffer {
        VkBuffer UniformBuffer;
        VkDeviceMemory UniformBufferMemory;
        void* UniformBufferMapped;
        u32 Binding;
        u32 Size;
    };

    void vulkan_copy_buffer(const std::shared_ptr<GraphicsDevice> device, VkBuffer src, VkBuffer dst, VkDeviceSize size);
    VulkanVertexBuffer vulkan_create_vertex_buffer(const std::shared_ptr<GraphicsDevice> device, const std::vector<VulkanVertex>& vertices);
    void vulkan_destroy_vertex_buffer(const std::shared_ptr<GraphicsDevice> device, const VulkanVertexBuffer& vertexBuffer);
    VulkanIndexBuffer vulkan_create_index_buffer(const std::shared_ptr<GraphicsDevice> device, const std::vector<VulkanIndex>& indices);
    void vulkan_destroy_index_buffer(const std::shared_ptr<GraphicsDevice> device, const VulkanIndexBuffer& indexBuffer);
    std::vector<VulkanUniformBuffer> vulkan_create_uniform_buffers(const std::shared_ptr<GraphicsDevice> device, u32 count);
}