#include "EnDjinnV.h"
#include "Buffer.h"


using namespace Djn::Gfx;


Buffer::Buffer(
    const Device& device,
    VkBufferUsageFlags usage,
    void* data,
    VkDeviceSize size) :
    vkDevice(device.GetLogical())
{
    if (size == 0) throw std::exception("Creating an empty buffer is not allowed.");

    // Create buffer.
    auto bufferCI = VkUtil::BufferCI();
    bufferCI.usage = usage;
    bufferCI.size = size;
    bufferCI.queueFamilyIndexCount = 0;
    bufferCI.pQueueFamilyIndices = NULL;
    bufferCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateBuffer(vkDevice, &bufferCI, NULL, &buffer);
    if (result != VK_SUCCESS) throw std::exception("Unable to create vertex buffer.");

    // Create memory.
    VkMemoryRequirements memoryReqs = {};
    vkGetBufferMemoryRequirements(vkDevice, buffer, &memoryReqs);

    memoryInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryInfo.pNext = NULL;
    memoryInfo.allocationSize = memoryReqs.size;
    if (!device.GetMemoryTypeIndex(
        memoryReqs.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        memoryInfo.memoryTypeIndex)) {
        vkDestroyBuffer(vkDevice, buffer, NULL);
        throw std::exception("Unable to locate correct memory type index for vertex buffer.");
    }
    result = vkAllocateMemory(vkDevice, &memoryInfo, NULL, &memory);
    if (result != VK_SUCCESS) {
        vkDestroyBuffer(vkDevice, buffer, NULL);
    }

    // Map, Copy, and Unmap
    uint32_t* vertexData;

    result = vkMapMemory(vkDevice, memory, 0, memoryInfo.allocationSize, NULL, (void**)&vertexData);
    if (result != VK_SUCCESS) throw std::exception("Failed to map memory for VBO.");

    memcpy(vertexData, data, size);

    vkUnmapMemory(vkDevice, memory);

    // Bind buffer and memory.
    // TODO: Can we bind buffer and memory before mapping and copying the data?
    result = vkBindBufferMemory(vkDevice, buffer, memory, 0);
    if (result != VK_SUCCESS) throw std::exception("Failed to bind memory for VBO.");
}


Buffer::~Buffer()
{
    vkFreeMemory(vkDevice, memory, NULL);
    vkDestroyBuffer(vkDevice, buffer, NULL);
}


VertexBuffer::VertexBuffer(const Device& device, std::vector<Vertex> vertices) :
    Buffer(
        device,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        vertices.data(),
        vertices.size() * sizeof(Vertex))
{}


VertexBuffer::~VertexBuffer()
{}


UniformBuffer::UniformBuffer(const Device& device) :
    Buffer(
        device,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        nullptr,
        (VkDeviceSize)0)
{}


UniformBuffer::~UniformBuffer()
{}
