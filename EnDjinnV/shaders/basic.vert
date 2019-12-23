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

    // 0 1 2  3 4 5    6 7 8  9 a b // vId
    // 0 0 0  1 1 1    2 2 2  3 3 3 // tId (vId/3)
    // 0 1 2  0 1 2    0 1 2  0 1 2 // tvId
    // 0 0 0  0 0 0    1 1 1  1 1 1 // fId
    // 0 1 2  3 4 5    0 1 2  3 4 5
    // 0 0 1  1 1 0    1 1 2  2 2 1 // x
    // 0 1 1  1 0 0    0 1 1  1 0 0 // y
    float triangleId = floor(gl_VertexIndex / 3);
    float faceId = floor(triangleId / 2);
    gl_Position.x = floor(gl_VertexIndex / 2);
    gl_Position.y = floor(gl_VertexIndex % 2);
    gl_Position.z = 0.25;
    gl_Position.w = 1;
    //gl_Position = mvp * pos;
}
