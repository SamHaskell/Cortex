#pragma once

#include "Cortex/Utils/Asserts.h"
#include "Cortex/Rendering/VulkanUtils.hpp"
#include "Cortex/Rendering/Vertex.hpp"
#include "Cortex/Rendering/Buffer.hpp"

#include <vector>

namespace Cortex
{
    // Eventually will hold reference to shaders/pipeline

    class StaticMesh
    {
        public:

        private:
            std::shared_ptr<RenderDevice> m_Device;
            std::vector<Vertex> m_Vertices;
            std::vector<u16> m_Indices;
            u32 m_VertexCount;
            u32 m_IndexCount;
            std::unique_ptr<Buffer> m_VertexBuffer;
            std::unique_ptr<Buffer> m_IndexBuffer;
    };
}