#pragma once


#include <vector>
#include <filesystem>
#include <iostream>
#include <fstream>


namespace Djn::Gfx
{

    class Shader
    {
    public:
        /// When the shader pipeline is setup propertly, they will not be compiled internally.
        /// When that happens, the spirv assembly will be loaded here.
        static std::vector<uint32_t> LoadBasicVertexShader()
        {
            namespace fs = std::filesystem;
            auto filepath = fs::current_path() / "shaders" / "basic.vert.spv";
            if (!fs::exists(filepath)) throw ShaderNotFoundException("Basic Vertex");

            return LoadFileContents(filepath);
        }
        static std::vector<uint32_t> LoadBasicFragmentShader()
        {
            namespace fs = std::filesystem;
            auto filepath = fs::current_path() / "shaders" / "basic.frag.spv";
            if (!fs::exists(filepath)) throw ShaderNotFoundException("Basic Fragment");

            return LoadFileContents(filepath);
        }
    private:
        static std::vector<uint32_t> LoadFileContents(std::filesystem::path filepath)
        {
            std::ifstream file;
            file.open(filepath, std::ios::in | std::ios::binary | std::ios::ate);
            std::vector<uint32_t> data(file.tellg());
            file.seekg(0, std::ios::beg);
            file.read((char*)data.data(), data.size());
            file.close();
            return data;
        }
    private:
        Shader() = default;

        class ShaderNotFoundException : Exception
        {
        public:
            ShaderNotFoundException(std::string shaderName) :
                Exception(std::string("Unable to locate shader: ") + shaderName)
            {}
        };
    };
}
