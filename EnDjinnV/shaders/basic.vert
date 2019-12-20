#version 400

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (std140, binding = 0) uniform bufferVals {
    mat4 view;
    mat4 proj;
} myBufferVals;

layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 color;
layout (location = 2) in vec4 normal;
layout (location = 3) in vec4 uv;
layout (location = 0) out vec4 outColor;

void main() {
    mat4 clip;
    clip[0] = vec4(1.0, 0.0, 0.0, 0.0);
    clip[1] = vec4(0.0, -1.0, 0.0, 0.0);
    clip[2] = vec4(0.0, 0.0, 0.5, 0.0);
    clip[3] = vec4(0.0, 0.0, 0.5, 1.0);
    outColor = color;
    gl_Position = clip * myBufferVals.view * myBufferVals.proj * pos;
}
