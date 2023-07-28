#pragma once

#include "Hazel/Renderer/Shader.h"
#include <glm/glm.hpp>

// TODO: REMOVE!
typedef unsigned int GLenum;

namespace Hazel {

    class OpenGLShader : public Shader
    {
    public:
        OpenGLShader(const std::string& filePath);//从文件中读取shader
        OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);//从字符串中读取shader
        virtual ~OpenGLShader();

        virtual void Bind() const override;//绑定shader
        virtual void Unbind() const override;//解绑shader

        virtual const std::string& GetName() const override { return m_Name; }//获取shader的名字

        void UploadUniformInt(const std::string& name, int value);//上传int类型的uniform

        void UploadUniformFloat(const std::string& name, float value);//上传float类型的uniform
        void UploadUniformFloat2(const std::string& name, const glm::vec2& value);//上传float2类型的uniform
        void UploadUniformFloat3(const std::string& name, const glm::vec3& value);//上传float3类型的uniform
        void UploadUniformFloat4(const std::string& name, const glm::vec4& value);//上传float4类型的uniform

        void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);//上传mat3类型的uniform
        void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);//上传mat4类型的uniform
    private:
        std::string ReadFile(const std::string& filePath);//从文件中读取shader
        std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);//预处理shader
        void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);//编译shader
    private:
        uint32_t m_RendererID;//shader的id
        std::string m_Name;//shader的名字
    };

}