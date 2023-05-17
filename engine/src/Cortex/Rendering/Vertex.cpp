#include "Cortex/Rendering/Vertex.hpp"

namespace Cortex
{
    VkVertexInputBindingDescription Vertex::GetBindingDescription()
    {
        VkVertexInputBindingDescription description = {};
        description.binding = 0;
        description.stride = sizeof(Vertex);
        description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return description;
    }

    std::array<VkVertexInputAttributeDescription, 2> Vertex::GetAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2> descriptions = {};
        descriptions[0].binding = 0;
        descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        descriptions[0].location = 0;
        descriptions[0].offset = offsetof(Vertex, Position);
        descriptions[1].binding = 0;
        descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        descriptions[1].location = 1;
        descriptions[1].offset = offsetof(Vertex, Color);
        return descriptions;
    }
}