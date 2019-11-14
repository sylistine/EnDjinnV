#include "EnDjinnV.h"
#include "CommandBuffer.h"


#include "VulkanUtil.h"


using namespace Djn::Gfx;


CommandBuffer::CommandBuffer(VkDevice device, VkCommandPool cmdPool) :
    vkDevice(device), vkCmdPool(cmdPool)
{
    count = 1;
    auto commandBufferAllocInfo = VkUtil::CommandBufferAllocInfo();
    commandBufferAllocInfo.commandPool = vkCmdPool;
    commandBufferAllocInfo.commandBufferCount = count;
    VkResult result = vkAllocateCommandBuffers(vkDevice, &commandBufferAllocInfo, &vkCommandBuffer);
    if (result != VK_SUCCESS) throw std::exception("Unable to allocate command buffer.");

    inited = true;
}


CommandBuffer::~CommandBuffer()
{
    FreeDeviceMemory();
}


void CommandBuffer::FreeDeviceMemory()
{
    if (!inited) return;

    VkCommandBuffer cmdBufferList[] = { vkCommandBuffer };
    vkFreeCommandBuffers(vkDevice, vkCmdPool, count, cmdBufferList);
}
