#include "EnDjinnV.h"
#include "CommandPool.h"


#include "VulkanUtil.h"


using namespace Djn::Gfx;


CommandPool::CommandPool(vk::Device device, uint32_t queueFamilyIndex, uint32_t initialBufferCount) :
    vkDevice(device)
{
    vk::CommandPoolCreateInfo commandPoolCI;
    commandPoolCI.queueFamilyIndex = queueFamilyIndex;
    vk::Result result = vkDevice.createCommandPool(&commandPoolCI, NULL, &pool);
    if (result != vk::Result::eSuccess) throw Exception("Unable to create command pool.");

    CreateCommandBuffers(initialBufferCount);

    inited = true;
}


CommandPool::~CommandPool()
{
    FreeDeviceMemory();
}


void CommandPool::CreateCommandBuffers(uint32_t count)
{
    if (count < 1) throw Exception("Cannot create 0 buffers.");
    
    FreeBuffers();

    buffers.resize(count);

    vk::CommandBufferAllocateInfo commandBufferAllocInfo;
    commandBufferAllocInfo.commandPool = pool;
    commandBufferAllocInfo.commandBufferCount = count;

    vk::Result result = vkDevice.allocateCommandBuffers(&commandBufferAllocInfo, buffers.data());
    if (result != vk::Result::eSuccess) throw Exception("Unable to allocate command buffer.");
}


void CommandPool::FreeBuffers()
{
    if (buffers.size() < 1) return;

    vkDevice.freeCommandBuffers(pool, (uint32_t)buffers.size(), buffers.data());
}


void CommandPool::FreeDeviceMemory() noexcept
{
    if (!inited) return;

    FreeBuffers();
    vkDevice.destroyCommandPool(pool);
    inited = false;
}
