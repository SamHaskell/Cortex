#pragma once

#include "Cortex/Utils/Asserts.h"
#include "Cortex/Rendering/VulkanUtils.hpp"
#include "Cortex/Rendering/Vertex.hpp"

#include <vector>

namespace Cortex
{
    // Eventually will hold reference to shaders/pipeline
    class StaticMesh
    {
        public:

        private:
            VkDevice m_DeviceHandle;
            std::vector<Vertex> m_Vertices;
            u32 m_VertexCount;
    };
}