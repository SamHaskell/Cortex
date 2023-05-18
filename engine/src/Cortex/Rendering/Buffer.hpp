#pragma once

#include "Cortex/Rendering/VulkanUtils.hpp"
#include "Cortex/Rendering/RenderDevice.hpp"
#include "Cortex/Rendering/Vertex.hpp"

#include <memory>
#include <vector>

namespace Cortex
{
    class Buffer
    {
    public:
        // Essentially just fills the createInfo for a vertex buffer in-place and hands it to the constructor
        // Unique pointer as this will be exclusively owned by a single mesh
        static std::unique_ptr<Buffer> CreateVertexBuffer(std::shared_ptr<RenderDevice> device, const std::vector<Vertex>& vertices);
        static std::unique_ptr<Buffer> CreateIndexBuffer(std::shared_ptr<RenderDevice> device, const std::vector<u16>& indices);

        Buffer(const std::shared_ptr<RenderDevice>& device, VkBufferCreateInfo createInfo);
        ~Buffer();

        Buffer(const Buffer &) = delete; // No copy pls
        Buffer &operator=(const Buffer &) = delete;

    private:
        VkDevice m_DeviceHandle;
        VkBuffer m_Buffer;
        VkDeviceMemory m_BufferMemory;
        VkDeviceSize m_BufferMemorySize;

        void CreateBuffer(const std::shared_ptr<RenderDevice> device, VkBufferCreateInfo& createInfo);
        void FillBuffer(void* pData);

    };
}