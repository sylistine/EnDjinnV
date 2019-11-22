#include "EnDjinnV.h"
#include "Vertex.h"


using namespace Djn::Gfx;


Vertex::Vertex() : position(double4(0, 0, 0, 0)), color(double4(0, 0, 0, 0))
{}


Vertex::Vertex(Djn::double4 position, Djn::double4 color) :
    position(position), color(color)
{}


Vertex::~Vertex()
{}
