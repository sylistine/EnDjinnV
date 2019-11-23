#include "EnDjinnV.h"
#include "Buffer.h"


using namespace Djn::Gfx;


Buffer::Buffer()
{}


Buffer::~Buffer()
{}


VertexBuffer::VertexBuffer(const Device& device, std::vector<Vertex> vertex)
{
    vkDevice = device.GetLogical();

    auto bufferCI = VkUtil::BufferCI();
    bufferCI.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferCI.size = vertex.size() * sizeof(Vertex);
    bufferCI.queueFamilyIndexCount = 0;
    bufferCI.pQueueFamilyIndices = NULL;
    bufferCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateBuffer(vkDevice, &bufferCI, NULL, &buffer);
    if (result != VK_SUCCESS) throw std::exception("Unable to create vertex buffer.");

    VkMemoryRequirements memoryReqs = {};
    vkGetBufferMemoryRequirements(vkDevice, buffer, &memoryReqs);

    VkMemoryAllocateInfo memoryAllocInfo = {};
    memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocInfo.pNext = NULL;
    memoryAllocInfo.allocationSize = memoryReqs.size;
    if (!device.GetMemoryTypeIndex(
        memoryReqs.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        memoryAllocInfo.memoryTypeIndex))
    {
        vkDestroyBuffer(vkDevice, buffer, NULL);
        throw std::exception("Unable to locate correct memory type index for vertex buffer.");
    }
}

VertexBuffer::~VertexBuffer()
{
    vkDestroyBuffer(vkDevice, buffer, NULL);
}