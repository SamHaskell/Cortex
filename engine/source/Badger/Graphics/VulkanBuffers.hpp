#pragma once

#include "Badger/Graphics/VulkanHelpers.hpp"
#include "Badger/Graphics/VulkanTypes.hpp"

namespace Badger {
    struct VulkanVertexBuffer {
        VkBuffer VertexBuffer;
        VkDeviceMemory VertexBufferMemory;
        u32 VertexCount;
    };

    VulkanVertexBuffer vulkan_create_vertex_buffer(VkPhysicalDevice physicalDevice, VkDevice device, const std::vector<VulkanVertex>& vertices);
    void vulkan_destroy_vertex_buffer(VkDevice device, const VulkanVertexBuffer& vertexBuffer);
}