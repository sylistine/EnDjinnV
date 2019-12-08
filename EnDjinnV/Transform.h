#pragma once


namespace Djn
{
    class Transform
    {
    public:
        Transform();
        Transform(vec3 pos, quat rot);
        // Getters/Setters.
        vec3 Position() const { return position; }
        void SetPosition(vec3 position) { this->position = position; }
        quat Rotation() const { return rotation; }
        void SetRotation(quat rotation) { this->rotation = rotation; }
        vec3 Scale() const { return scale; }
        void SetScale(vec3 scale) { this->scale = scale; }
    private:
        vec3 position;
        quat rotation;
        vec3 scale;
    };
}
