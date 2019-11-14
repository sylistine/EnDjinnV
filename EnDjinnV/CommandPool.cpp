#include "EnDjinnV.h"
#include "CommandPool.h"


#include "VulkanUtil.h"


using namespace Djn::Gfx;


CommandPool::CommandPool(VkDevice device, uint32_t gfxQueueFamilyIdx) : vkDevice(device)
{
    auto commandPoolCI = VkUtil::CommandPoolCI();
    commandPoolCI.queueFamilyIndex = gfxQueueFamilyIdx;
    VkResult result = vkCreateCommandPool(vkDevice, &commandPoolCI, NULL, &vkCommandPool);
    if (result != VK_SUCCESS) throw std::exception("Unable to create command pool.");

    inited = true;
}


CommandPool::~CommandPool()
{
    FreeDeviceMemory();
}


void CommandPool::FreeDeviceMemory()
{
    if (!inited) return;

    vkDestroyCommandPool(vkDevice, vkCommandPool, NULL);
    inited = false;
}