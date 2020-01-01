#version 400

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (std140, binding = 0) uniform cameraValues {
    mat4 mvp;
} camera;

layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 color;
layout (location = 2) in vec4 normal;
layout (location = 3) in vec4 uv;
layout (location = 0) out vec4 outColor;

void main() {
    outColor = color;
    mat4 clip = mat4(1.0f, 0.0f, 0.0f, 0.0f,
                     0.0f,-1.0f, 0.0f, 0.0f,
                     0.0f, 0.0f, 0.5f, 0.0f,
                     0.0f, 0.0f, 0.5f, 1.0f);
    gl_Position = camera.mvp * pos;
}
