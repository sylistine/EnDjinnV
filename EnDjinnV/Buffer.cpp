#include "EnDjinnV.h"
#include "Buffer.h"


using namespace Djn::Gfx;


Buffer::Buffer(
    const Device& device,
    vk::BufferUsageFlags usage,
    void* data,
    vk::DeviceSize size) :
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
    vk::MemoryRequirements memoryReqs;
    d.getBufferMemoryRequirements(buffer, &memoryReqs);

    memoryAllocationSize = memoryReqs.size;

    vk::MemoryAllocateInfo memoryAI;
    memoryAI.allocationSize = memoryAllocationSize;
    memoryAI.memoryTypeIndex = UINT32_MAX;
    if (!device.GetMemoryTypeIndex(
        memoryReqs.memoryTypeBits,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        memoryAI.memoryTypeIndex))
    {
        d.destroyBuffer(buffer, NULL);
        throw Exception("Unable to locate correct memory type index for vertex buffer.");
    }
    result = d.allocateMemory(&memoryAI, NULL, &memory);
    if (result != vk::Result::eSuccess) {
        d.destroyBuffer(buffer, NULL);
    }

    updateData(data, size);

    // Bind buffer and memory.
    // TODO: Can we bind buffer and memory before mapping and copying the data?
    d.bindBufferMemory(buffer, memory, 0);

    inited = true;
}


Buffer::~Buffer()
{
    FreeMemory();
}


void Buffer::updateData(void* data, vk::DeviceSize size)
{
    vk::DeviceSize offset(0);
    vk::MemoryMapFlags flags(0);

    auto vertexDataDst = d.mapMemory(
        memory,
        offset,
        memoryAllocationSize,
        flags);
    memcpy(vertexDataDst, data, size);
    d.unmapMemory(memory);
}


void Buffer::FreeMemory()
{
    if (!inited) return;

    d.destroyBuffer(buffer, NULL);
    d.freeMemory(memory, NULL);

    inited = false;
}
