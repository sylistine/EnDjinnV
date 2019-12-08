#include "EnDjinnV.h"
#include "Transform.h"


using namespace Djn;


Transform::Transform() : position(vec3(0, 0, 0)), rotation(), scale(vec3(1, 1, 1))
{

}


Transform::Transform(vec3 pos, quat rot) : position(pos), rotation(rot), scale(vec3(1, 1, 1))
{

}
