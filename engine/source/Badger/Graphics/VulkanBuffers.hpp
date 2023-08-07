#pragma once

#include "Badger/Graphics/VulkanHelpers.hpp"
#include "Badger/Graphics/VulkanTypes.hpp"

#include "Badger/Graphics/GraphicsDevice.hpp"

namespace Badger {
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

    void vulkan_copy_buffer(const std::shared_ptr<GraphicsDevice> device, VkBuffer src, VkBuffer dst, VkDeviceSize size);
    VulkanVertexBuffer vulkan_create_vertex_buffer(const std::shared_ptr<GraphicsDevice> device, const std::vector<VulkanVertex>& vertices);
    void vulkan_destroy_vertex_buffer(const std::shared_ptr<GraphicsDevice> device, const VulkanVertexBuffer& vertexBuffer);
    VulkanIndexBuffer vulkan_create_index_buffer(const std::shared_ptr<GraphicsDevice> device, const std::vector<VulkanIndex>& indices);
    void vulkan_destroy_index_buffer(const std::shared_ptr<GraphicsDevice> device, const VulkanIndexBuffer& indexBuffer);
}