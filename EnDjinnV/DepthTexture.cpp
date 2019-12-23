#include "EnDjinnV.h"
#include "DepthTexture.h"

#include "VulkanUtil.h"


using namespace Djn::Gfx;


DepthTexture::DepthTexture(const Device& device, uint32_t width, uint32_t height) :
    vkDevice(device.GetLogical())
{
    vk::ImageCreateInfo imageCI;
    imageCI.imageType = vk::ImageType::e2D;
    imageCI.format = vk::Format::eD16Unorm;
    imageCI.extent.width = width;
    imageCI.extent.height = height;
    imageCI.extent.depth = 1;
    imageCI.mipLevels = 1;
    imageCI.arrayLayers = 1;
    imageCI.samples = vk::SampleCountFlagBits::e1;
    imageCI.initialLayout = vk::ImageLayout::eUndefined;
    imageCI.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
    imageCI.queueFamilyIndexCount = 0;
    imageCI.pQueueFamilyIndices = NULL;
    imageCI.sharingMode = vk::SharingMode::eExclusive;
    vk::Result result = vkDevice.createImage(&imageCI, NULL, &image);
    if (result != vk::Result::eSuccess) throw Exception("Failed to create VkImage.");

    vk::MemoryRequirements memoryReqs;
    vkDevice.getImageMemoryRequirements(image, &memoryReqs);

    uint32_t memoryTypeIdx = UINT32_MAX;
    if (!device.GetMemoryTypeIndex(memoryReqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, memoryTypeIdx)) {
        vkDevice.destroyImage(image, NULL);
        throw Exception("Couldn't locate required memory type idx.");
    }

    vk::MemoryAllocateInfo memoryAllocateInfo;
    memoryAllocateInfo.allocationSize = memoryReqs.size;
    memoryAllocateInfo.memoryTypeIndex = memoryTypeIdx;

    result = vkDevice.allocateMemory(&memoryAllocateInfo, NULL, &memory);
    if (result != vk::Result::eSuccess) {
        vkDestroyImage(vkDevice, image, NULL);
        throw new std::exception("Couldn't allocate memory for image.");
    }

    vkDevice.bindImageMemory(image, memory, 0);

    vk::ImageViewCreateInfo viewCreateInfo;
    viewCreateInfo.image = image;
    viewCreateInfo.format = vk::Format::eD16Unorm;
    viewCreateInfo.components.r = vk::ComponentSwizzle::eR;
    viewCreateInfo.components.g = vk::ComponentSwizzle::eG;
    viewCreateInfo.components.b = vk::ComponentSwizzle::eB;
    viewCreateInfo.components.a = vk::ComponentSwizzle::eA;
    viewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
    viewCreateInfo.subresourceRange.baseMipLevel = 0;
    viewCreateInfo.subresourceRange.levelCount = 1;
    viewCreateInfo.subresourceRange.baseArrayLayer = 0;
    viewCreateInfo.subresourceRange.layerCount = 1;
    viewCreateInfo.viewType = vk::ImageViewType::e2D;
    result = vkDevice.createImageView(&viewCreateInfo, NULL, &view);
    if (result != vk::Result::eSuccess) {
        vkFreeMemory(vkDevice, memory, NULL);
        vkDestroyImage(vkDevice, image, NULL);
        throw new std::exception("Unable to create Image image view.");
    }
}


DepthTexture::~DepthTexture()
{
    vkDestroyImageView(vkDevice, view, NULL);
    vkFreeMemory(vkDevice, memory, NULL);
    vkDestroyImage(vkDevice, image, NULL);
}
