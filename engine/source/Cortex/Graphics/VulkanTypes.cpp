#include "Cortex/Graphics/VulkanTypes.hpp"

namespace Cortex {
    std::vector<VkVertexInputBindingDescription> VulkanVertex::BindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        bindingDescriptions[0].stride = sizeof(VulkanVertex);
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> VulkanVertex::AttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(4);
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].offset = offsetof(VulkanVertex, Position);
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].offset = offsetof(VulkanVertex, Normal);
        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].offset = offsetof(VulkanVertex, Color);
        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].offset = offsetof(VulkanVertex, TexCoord);

        return attributeDescriptions;
    }
}