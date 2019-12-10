#include "Vertex.h"
#include "EnDjinnV.h"

using namespace Djn::Gfx;

Vertex::Vertex() : position(vec4(0, 0, 0, 0)), color(vec4(0, 0, 0, 0)) {}

Vertex::Vertex(vec4 position, vec4 color) : position(position), color(color) {}

Vertex::~Vertex() {}

std::vector<VkVertexInputAttributeDescription> Vertex::AttributeDescs() {
  std::vector<VkVertexInputAttributeDescription> attributes;

  VkVertexInputAttributeDescription positionAttribute;
  positionAttribute.location = 0;
  positionAttribute.binding = 0;
  positionAttribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
  positionAttribute.offset = 0;
  attributes.push_back(positionAttribute);

  VkVertexInputAttributeDescription normalAttribute;
  normalAttribute.location = 2;
  normalAttribute.binding = 0;
  normalAttribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
  normalAttribute.offset = 16;
  attributes.push_back(normalAttribute);

  VkVertexInputAttributeDescription uvAttribute;
  uvAttribute.location = 3;
  uvAttribute.binding = 0;
  uvAttribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
  uvAttribute.offset = 32;
  attributes.push_back(uvAttribute);

  VkVertexInputAttributeDescription colorAttribute;
  colorAttribute.location = 4;
  colorAttribute.binding = 0;
  colorAttribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
  colorAttribute.offset = 48;
  attributes.push_back(colorAttribute);

  return attributes;
}
