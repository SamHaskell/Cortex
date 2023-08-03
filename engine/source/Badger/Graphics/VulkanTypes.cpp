#include "Badger/Graphics/VulkanTypes.hpp"

namespace Badger {
    std::vector<VkVertexInputBindingDescription> VulkanVertex::BindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        bindingDescriptions[0].stride = sizeof(VulkanVertex);
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> VulkanVertex::AttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].offset = offsetof(VulkanVertex, Position);
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].offset = offsetof(VulkanVertex, Color);
        return attributeDescriptions;
    }

    std::vector<VkVertexInputBindingDescription> VulkanVertex2D::BindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        bindingDescriptions[0].stride = sizeof(VulkanVertex2D);
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> VulkanVertex2D::AttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(1);
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].offset = offsetof(VulkanVertex2D, Position);
        return attributeDescriptions;
    }
}