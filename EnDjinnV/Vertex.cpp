#include "EnDjinnV.h"
#include "Vertex.h"


using namespace Djn::Gfx;


Vertex::Vertex() : position(vec4(0, 0, 0, 0)), color(vec4(0, 0, 0, 0))
{}


Vertex::Vertex(vec4 position, vec4 color) :
    position(position), color(color)
{}


Vertex::~Vertex()
{}
