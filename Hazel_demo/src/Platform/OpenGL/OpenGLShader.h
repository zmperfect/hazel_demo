#pragma once

#include "Hazel/Renderer/Shader.h"
#include <glm/glm.hpp>

// TODO: REMOVE!
typedef unsigned int GLenum;

namespace Hazel {

    class OpenGLShader : public Shader
    {
    public:
        OpenGLShader(const std::string& filePath);//���ļ��ж�ȡshader
        OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);//���ַ����ж�ȡshader
        virtual ~OpenGLShader();

        virtual void Bind() const override;//��shader
        virtual void Unbind() const override;//���shader

        virtual const std::string& GetName() const override { return m_Name; }//��ȡshader������

        void UploadUniformInt(const std::string& name, int value);//�ϴ�int���͵�uniform

        void UploadUniformFloat(const std::string& name, float value);//�ϴ�float���͵�uniform
        void UploadUniformFloat2(const std::string& name, const glm::vec2& value);//�ϴ�float2���͵�uniform
        void UploadUniformFloat3(const std::string& name, const glm::vec3& value);//�ϴ�float3���͵�uniform
        void UploadUniformFloat4(const std::string& name, const glm::vec4& value);//�ϴ�float4���͵�uniform

        void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);//�ϴ�mat3���͵�uniform
        void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);//�ϴ�mat4���͵�uniform
    private:
        std::string ReadFile(const std::string& filePath);//���ļ��ж�ȡshader
        std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);//Ԥ����shader
        void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);//����shader
    private:
        uint32_t m_RendererID;//shader��id
        std::string m_Name;//shader������
    };

}