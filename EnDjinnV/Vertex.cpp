#include "EnDjinnV.h"
#include "Vertex.h"


using namespace Djn::Gfx;


Vertex::Vertex() :
    position(vec4(0, 0, 0, 0)),
    color(vec4(0, 0, 0, 0)),
    normal(vec4(0, 1, 0, 0)),
    uv(vec4(0, 0, 0, 0))
{}


Vertex::Vertex(vec4 position, vec4 color) :
    position(position),
    color(color),
    normal(vec4(0, 1, 0, 0)),
    uv(vec4(0, 0, 0, 0))
{}


Vertex::~Vertex()
{}


std::vector<vk::VertexInputAttributeDescription> Vertex::AttributeDescs()
{
    std::vector<vk::VertexInputAttributeDescription> attributes;

    vk::VertexInputAttributeDescription positionAttribute;
    positionAttribute.location = 0;
    positionAttribute.binding = 0;
    positionAttribute.format = vk::Format::eR32G32B32A32Sfloat;
    positionAttribute.offset = 0;
    attributes.push_back(positionAttribute);

    vk::VertexInputAttributeDescription colorAttribute;
    colorAttribute.location = 1;
    colorAttribute.binding = 0;
    colorAttribute.format = vk::Format::eR32G32B32A32Sfloat;
    colorAttribute.offset = 16;
    attributes.push_back(colorAttribute);

    vk::VertexInputAttributeDescription normalAttribute;
    normalAttribute.location = 2;
    normalAttribute.binding = 0;
    normalAttribute.format = vk::Format::eR32G32B32A32Sfloat;
    normalAttribute.offset = 32;
    attributes.push_back(normalAttribute);

    // TODO: uvs only need to be a float2
    vk::VertexInputAttributeDescription uvAttribute;
    uvAttribute.location = 3;
    uvAttribute.binding = 0;
    uvAttribute.format = vk::Format::eR32G32B32A32Sfloat;
    uvAttribute.offset = 48;
    attributes.push_back(uvAttribute);

    return attributes;
}
