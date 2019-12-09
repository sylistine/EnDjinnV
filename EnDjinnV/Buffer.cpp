#include "EnDjinnV.h"
#include "Buffer.h"


using namespace Djn::Gfx;


Buffer::Buffer(
    const Device& device,
    vk::BufferUsageFlags usage,
    void* data,
    VkDeviceSize size) :
    d(device.GetLogical())
{
    if (size == 0) throw Exception("Creating an empty buffer is not allowed.");

    // Create buffer.
    vk::BufferCreateInfo bufferCI;
    bufferCI.usage = usage;
    bufferCI.size = size;
    bufferCI.queueFamilyIndexCount = 0;
    bufferCI.pQueueFamilyIndices = NULL;
    bufferCI.sharingMode = vk::SharingMode::eExclusive;

    vk::Result result = d.createBuffer(&bufferCI, NULL, &buffer);
    if (result != vk::Result::eSuccess) throw Exception("Unable to create vertex buffer.");

    // Create memory.
    vk::MemoryRequirements memoryReqs = {};
    d.getBufferMemoryRequirements(buffer, &memoryReqs);

    memoryInfo.allocationSize = memoryReqs.size;
    if (!device.GetMemoryTypeIndex(
        memoryReqs.memoryTypeBits,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        memoryInfo.memoryTypeIndex))
    {
        d.destroyBuffer(buffer, NULL);
        throw Exception("Unable to locate correct memory type index for vertex buffer.");
    }
    result = d.allocateMemory(&memoryInfo, NULL, &memory);
    if (result != vk::Result::eSuccess) {
        d.destroyBuffer(buffer, NULL);
    }

    // Map, Copy, and Unmap
    auto vertexDataDst = d.mapMemory(
        memory,
        vk::DeviceSize(0), memoryInfo.allocationSize,
        vk::MemoryMapFlags(0));
    memcpy(vertexDataDst, data, size);
    d.unmapMemory(memory);

    // Bind buffer and memory.
    // TODO: Can we bind buffer and memory before mapping and copying the data?
    d.bindBufferMemory(buffer, memory, 0);
}


Buffer::~Buffer()
{
    d.freeMemory(memory, NULL);
    d.destroyBuffer(buffer, NULL);
}
