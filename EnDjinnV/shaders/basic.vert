#version 400

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (std140, binding = 0) uniform timeValues {
    float timeSinceStart;
} time;

layout (std140, binding = 1) uniform cameraValues {
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
    float t = time.timeSinceStart;
    vec4 p = pos;
    p.xy += vec2(sin(t), cos(t)) * 2.0;
    vec4 oPos = camera.mvp * p;
    gl_Position = oPos;
}
