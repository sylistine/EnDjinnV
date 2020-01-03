#version 400

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (std140, binding = 0) uniform cameraValues {
    mat4 clip;
    mat4 proj;
    mat4 view;
    mat4 model;
    mat4 mvp;
} camera;

layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 color;
layout (location = 2) in vec4 normal;
layout (location = 3) in vec4 uv;
layout (location = 0) out vec4 outColor;

//#define flip(m) transpose(m)
#define flip(m) m

void main() {
    outColor = color;
    mat4 m = flip(camera.model);
    mat4 v = flip(camera.view);
    mat4 p = flip(camera.proj);
    mat4 c = flip(camera.clip);
    mat4 cpvm = c * p * v * m;
    mat4 mvpc = m * v * p * c;
    gl_Position = cpvm * pos;
}
