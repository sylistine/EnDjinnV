#include "EnDjinnV.h"
#include "Buffer.h"


using namespace Djn::Gfx;


Buffer::Buffer()
{}


Buffer::~Buffer()
{}


VertexBuffer::VertexBuffer(const Device& device, std::vector<Vertex> vertices)
{
    if (vertices.size() == 0) throw std::exception("Creating an empty vertex buffer is not allowed.");

    VkResult result;

    vkDevice = device.GetLogical();

    auto bufferCI = VkUtil::BufferCI();
    bufferCI.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferCI.size = vertices.size() * sizeof(Vertex);
    bufferCI.queueFamilyIndexCount = 0;
    bufferCI.pQueueFamilyIndices = NULL;
    bufferCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    result = vkCreateBuffer(vkDevice, &bufferCI, NULL, &buffer);
    if (result != VK_SUCCESS) throw std::exception("Unable to create vertex buffer.");

    VkMemoryRequirements memoryReqs = {};
    vkGetBufferMemoryRequirements(vkDevice, buffer, &memoryReqs);

    VkMemoryAllocateInfo mallocInfo = {};
    mallocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mallocInfo.pNext = NULL;
    mallocInfo.allocationSize = memoryReqs.size;
    if (!device.GetMemoryTypeIndex(
        memoryReqs.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        mallocInfo.memoryTypeIndex))
    {
        vkDestroyBuffer(vkDevice, buffer, NULL);
        throw std::exception("Unable to locate correct memory type index for vertex buffer.");
    }
    vkAllocateMemory(vkDevice, &mallocInfo, NULL, &memory);

    result = vkMapMemory(vkDevice, memory, 0, memoryReqs.size, NULL, (void**)&vertexData);
    if (result != VK_SUCCESS) {
        vkFreeMemory(vkDevice, memory, NULL);
        vkDestroyBuffer(vkDevice, buffer, NULL);
        throw std::exception("Failed to map memory for VBO.");
    }
    memcpy(vertexData, vertices.data(), vertices.size() * sizeof(Vertex));
    vkUnmapMemory(vkDevice, memory);
    result = vkBindBufferMemory(vkDevice, buffer, memory, 0);
    if (result != VK_SUCCESS) {
        vkFreeMemory(vkDevice, memory, NULL);
        vkDestroyBuffer(vkDevice, buffer, NULL);
        throw std::exception("Failed to bind memory for VBO.");
    }
}


VertexBuffer::~VertexBuffer()
{
    vkFreeMemory(vkDevice, memory, NULL);
    vkDestroyBuffer(vkDevice, buffer, NULL);
}
