#include "EnDjinnV.h"
#include "DepthTexture.h"

#include "VulkanUtil.h"


using namespace Djn;


DepthTexture::DepthTexture(
    VkDevice device,
    uint32_t initialWidth,
    uint32_t initialHeight,
    VkPhysicalDeviceMemoryProperties memoryProperties)
{
    VkResult result;

    auto imageCreateInfo = VkUtil::ImageCreateInfo();
    imageCreateInfo.pNext = NULL;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = VK_FORMAT_D16_UNORM;
    imageCreateInfo.extent.width = initialWidth;
    imageCreateInfo.extent.height = initialHeight;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageCreateInfo.queueFamilyIndexCount = 0;
    imageCreateInfo.pQueueFamilyIndices = NULL;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.flags = 0;
    result = vkCreateImage(device, &imageCreateInfo, NULL, &image);
    if (result != VK_SUCCESS) throw new std::exception("Failed to create VkImage.");

    VkMemoryRequirements memoryReqs;
    vkGetImageMemoryRequirements(device, image, &memoryReqs);
    uint32_t typeBits = memoryReqs.memoryTypeBits;
    uint32_t memoryTypeIdx = UINT32_MAX;
    VkFlags memPropDevLocalBit = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    for (auto i = 0u; i < memoryProperties.memoryTypeCount; i++) {
        if ((typeBits & 1) == 1) {
            if ((memoryProperties.memoryTypes[i].propertyFlags & memPropDevLocalBit) == memPropDevLocalBit) {
                memoryTypeIdx = i;
                break;
            }
        }
        typeBits >>= 1;
    }
    if (memoryTypeIdx == UINT32_MAX) throw new std::exception("Couldn't locate required memory type idx.");

    auto memoryAllocateInfo = VkUtil::MemoryAllocateInfo();
    memoryAllocateInfo.allocationSize = memoryReqs.size;
    memoryAllocateInfo.memoryTypeIndex = memoryTypeIdx;

    result = vkAllocateMemory(device, &memoryAllocateInfo, NULL, &memory);
    if (result != VK_SUCCESS) throw new std::exception("Couldn't allocate memory for image.");

    result = vkBindImageMemory(device, image, memory, 0);
    if (result != VK_SUCCESS) throw new std::exception("Unable to bind Image to DeviceMemory.");

    auto viewCreateInfo = VkUtil::ImageViewCreateInfo();
    viewCreateInfo.image = image;
    viewCreateInfo.format = VK_FORMAT_D16_UNORM;
    viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    viewCreateInfo.subresourceRange.baseMipLevel = 0;
    viewCreateInfo.subresourceRange.levelCount = 1;
    viewCreateInfo.subresourceRange.baseArrayLayer = 0;
    viewCreateInfo.subresourceRange.layerCount = 1;
    viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    vkCreateImageView(device, &viewCreateInfo, NULL, &view);
}


DepthTexture::~DepthTexture()
{}
