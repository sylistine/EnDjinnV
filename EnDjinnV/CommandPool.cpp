#include "EnDjinnV.h"
#include "CommandPool.h"


#include "VulkanUtil.h"


using namespace Djn::Gfx;


CommandPool::CommandPool(VkDevice device, uint32_t queueFamilyIndex, uint32_t initialBufferCount) :
    vkDevice(device)
{
    auto commandPoolCI = VkUtil::CommandPoolCI();
    commandPoolCI.queueFamilyIndex = queueFamilyIndex;
    VkResult result = vkCreateCommandPool(vkDevice, &commandPoolCI, NULL, &pool);
    if (result != VK_SUCCESS) throw std::exception("Unable to create command pool.");

    CreateCommandBuffer(initialBufferCount);

    inited = true;
}


CommandPool::~CommandPool()
{
    FreeDeviceMemory();
}


VkCommandBuffer CommandPool::CreateCommandBuffer(uint32_t count)
{
    if (count < 1) throw Exception("Cannot create 0 buffers.");

    VkCommandBuffer newBuffer;
    auto commandBufferAllocInfo = VkUtil::CommandBufferAllocInfo();
    commandBufferAllocInfo.commandPool = pool;
    commandBufferAllocInfo.commandBufferCount = count;
    VkResult result = vkAllocateCommandBuffers(vkDevice, &commandBufferAllocInfo, &newBuffer);
    if (result != VK_SUCCESS) throw Exception("Unable to allocate command buffer.");

    buffers.push_back(newBuffer);
    return newBuffer;
}


void CommandPool::FreeDeviceMemory() noexcept
{
    if (!inited) return;

    vkFreeCommandBuffers(vkDevice, pool, buffers.size(), buffers.data());
    vkDestroyCommandPool(vkDevice, pool, NULL);
    inited = false;
}
