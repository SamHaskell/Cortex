#pragma once

#include "Cortex/Rendering/VulkanUtils.hpp"
#include "Cortex/Rendering/RenderDevice.hpp"
#include "Cortex/Rendering/Vertex.hpp"

namespace Cortex
{
    struct BufferConfig {

    };

    class Buffer
    {
    public:
        static Buffer CreateVertexBuffer(const std::unique_ptr<RenderDevice>& device, const std::vector<Vertex>& vertices);
        ~Buffer();

        Buffer(const Buffer &) = delete; // No copy pls
        Buffer &operator=(const Buffer &) = delete;

    private:
        VkDevice m_DeviceHandle;
        VkBuffer m_Buffer;
        VkDeviceMemory m_BufferMemory;

        Buffer(const std::unique_ptr<RenderDevice>& device, const std::vector<Vertex>& vertices, BufferConfig config); // Hidey-hidey
    };
}