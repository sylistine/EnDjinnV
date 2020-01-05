#pragma once


namespace Djn
{
    class Transform
    {
    public:
        Transform();
        Transform(vec3 pos, quat rot);
        vec3 getPosition() const { return position; }
        void setPosition(vec3 position) { this->position = position; }
        quat getRotation() const { return rotation; }
        void setRotation(quat rotation) { this->rotation = rotation; }
        vec3 getScale() const { return scale; }
        void setScale(vec3 scale) { this->scale = scale; }
    private:
        vec3 position;
        quat rotation;
        vec3 scale;
    };
}
