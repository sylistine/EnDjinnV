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
        static std::vector<char> LoadBasicVertexShader()
        {
            namespace fs = std::filesystem;
            auto filepath = fs::current_path() / "shaders" / "basicVert.spv";
            if (!fs::exists(filepath)) throw ShaderNotFoundException("Basic Vertex");
            return LoadFileContents(filepath);
        }
        static std::vector<char> LoadBasicFragmentShader()
        {
            namespace fs = std::filesystem;
            auto filepath = fs::current_path() / "shaders" / "basicFrag.spv";
            if (!fs::exists(filepath)) throw ShaderNotFoundException("Basic Fragment");
            return LoadFileContents(filepath);
        }
    private:
        static std::vector<char> LoadFileContents(std::filesystem::path filepath)
        {
            std::ifstream file;
            file.open(filepath, std::ios::in | std::ios::binary | std::ios::ate);
            std::vector<char> data(file.tellg());
            file.seekg(0, std::ios::beg);
            file.read(data.data(), data.size());
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
