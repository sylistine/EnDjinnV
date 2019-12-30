#version 400

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (std140, binding = 0) uniform bufferVals {
    mat4 mvp;
} myBufferVals;

layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 color;
layout (location = 2) in vec4 normal;
layout (location = 3) in vec4 uv;
layout (location = 0) out vec4 outColor;

void main() {
    outColor = color;

    mat4 mvp;
    mvp[0] = vec4(2.159338, 0.279808, -0.521391, 0.0);
    mvp[1] = vec4(0.0, 2.331730, 0.312834, 0.0);
    mvp[2] = vec4(0.448109, -0.232264, 5.720718, -1.0);
    mvp[3] = vec4(0.089532, -0.046406, 0.086473, 0.0);

    gl_Position = mvp * pos;
}
