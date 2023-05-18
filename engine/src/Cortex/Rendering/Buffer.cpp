#include "Cortex/Rendering/Buffer.hpp"

namespace Cortex
{
    Buffer::Buffer(const std::shared_ptr<RenderDevice> &device, VkBufferCreateInfo createInfo)
        : m_DeviceHandle(device->GetDevice())
    {
        CreateBuffer(device, createInfo);
    }

    Buffer::~Buffer()
    {
        vkDestroyBuffer(m_DeviceHandle, m_Buffer, nullptr);
        vkFreeMemory(m_DeviceHandle, m_BufferMemory, nullptr);
    }

    std::unique_ptr<Buffer> Buffer::CreateVertexBuffer(std::shared_ptr<RenderDevice> device, const std::vector<Vertex> &vertices)
    {
        VkBufferCreateInfo bufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        bufferInfo.size = sizeof(vertices[0]) * vertices.size();
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        auto buffer = std::make_unique<Buffer>(device, bufferInfo);
        buffer->FillBuffer((void*)vertices.data());
        return buffer;
    }

    std::unique_ptr<Buffer> Buffer::CreateIndexBuffer(std::shared_ptr<RenderDevice> device, const std::vector<u16> &indices)
    {
        VkBufferCreateInfo bufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        bufferInfo.size = sizeof(indices[0]) * indices.size();
        bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        return std::make_unique<Buffer>(device, bufferInfo);
    }

    void Buffer::CreateBuffer(const std::shared_ptr<RenderDevice> device, VkBufferCreateInfo &createInfo)
    {
        VkResult result = vkCreateBuffer(m_DeviceHandle, &createInfo, nullptr, &m_Buffer);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a Vulkan Buffer Object");

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(m_DeviceHandle, m_Buffer, &memoryRequirements);

        VkMemoryAllocateInfo allocInfo = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = device->FindMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        result = vkAllocateMemory(m_DeviceHandle, &allocInfo, nullptr, &m_BufferMemory);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to allocate device memory for a Vulkan Buffer Object");

        vkBindBufferMemory(m_DeviceHandle, m_Buffer, m_BufferMemory, 0);
    }

    void Buffer::FillBuffer(void* pData)
    {
        void *data;
        vkMapMemory(m_DeviceHandle, m_BufferMemory, 0, m_BufferMemorySize, 0, &data);
        memcpy(data, pData, (size_t)m_BufferMemorySize);
        vkUnmapMemory(m_DeviceHandle, m_BufferMemory);
    }
}