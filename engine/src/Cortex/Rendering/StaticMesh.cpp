#include "Cortex/Rendering/StaticMesh.hpp"

namespace Cortex {
    StaticMesh::StaticMesh(std::shared_ptr<RenderDevice> device, std::vector<Vertex> vertices, std::vector<u16> indices)
    : m_Device(device), m_Vertices(vertices), m_Indices(indices)
    {
        m_VertexCount = static_cast<u32>(m_Vertices.size());
        m_IndexCount = static_cast<u32>(m_Indices.size());
        m_VertexBuffer = Buffer::CreateVertexBuffer(m_Device, m_Vertices);
        m_IndexBuffer = Buffer::CreateIndexBuffer(m_Device, m_Indices);
    }
}