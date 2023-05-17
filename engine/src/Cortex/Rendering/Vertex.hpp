#pragma once

#include "Cortex/Utils/Asserts.h"
#include "Cortex/Rendering/VulkanUtils.hpp"

#include "glm/glm.hpp"

#include <array>

namespace Cortex
{
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Color;

        static VkVertexInputBindingDescription GetBindingDescription();
        static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions();
    };
}