#pragma once

#include <shaderc/shaderc.h>

namespace Djn::Gfx
{
    typedef struct Shader
    {
        Shader(const char* name, const char* shader) :
            name(name), shader(shader), kind(shaderc_glsl_infer_from_source)
        {};
        Shader(const char* name, const char* shader, shaderc_shader_kind kind) :
            name(name), shader(shader), kind(kind)
        {};
        const char* name;
        const char* shader;
        shaderc_shader_kind kind;
    } Shader;

    typedef struct VertexShader : public Shader
    {
        VertexShader(const char* name, const char* shader) : Shader(name, shader, shaderc_vertex_shader)
        {};
    };

    typedef struct FragmentShader : public Shader
    {
        FragmentShader(const char* name, const char* shader) : Shader(name, shader, shaderc_fragment_shader)
        {};
    };


    Shader BasicVertexShader = VertexShader(
        "Basic Vertex Shader",
        "#version 400\n"
        "#extension GL_ARB_separate_shader_objects : enable\n"
        "#extension GL_ARB_shading_language_420pack : enable\n"
        "layout (std140, binding = 0) uniform bufferVals {\n"
        "    mat4 mvp;\n"
        "} myBufferVals;\n"
        "layout (location = 0) in vec4 pos;\n"
        "layout (location = 1) in vec4 inColor;\n"
        "layout (location = 0) out vec4 outColor;\n"
        "void main() {\n"
        "   outColor = inColor;\n"
        "   gl_Position = myBufferVals.mvp * pos;\n"
        "}\n");
    Shader BasicFragmentShader = FragmentShader(
        "Basic Fragment Shader",
        "#version 400\n"
        "#extension GL_ARB_separate_shader_objects : enable\n"
        "#extension GL_ARB_shading_language_420pack : enable\n"
        "layout (location = 0) in vec4 color;\n"
        "layout (location = 0) out vec4 outColor;\n"
        "void main() {\n"
        "   outColor = color;\n"
        "}\n");
}
