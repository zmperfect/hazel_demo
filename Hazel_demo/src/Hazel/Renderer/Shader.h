#pragma once

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

namespace Hazel {

    class Shader
    {
    public:
        virtual ~Shader() = default;//ʹ��Ĭ�ϵ���������

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void SetInt(const std::string& name, int value) = 0;//����intֵ
        virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;//����int����
        virtual void SetFloat(const std::string& name, float value) = 0;
        virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;//����float3ֵ
        virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;//����float4ֵ
        virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;//����mat4ֵ

        virtual const std::string& GetName() const = 0;//��ȡshader������

        static Ref<Shader> Create(const std::string& filepath);//�����ļ�·������shader
        static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);//�����ַ�������shader
    };

    class ShaderLibrary
    {
    public:
        void Add(const std::string& name, const Ref<Shader>& shader);//���shader
        void Add(const Ref<Shader>& shader);//���shader
        Ref<Shader> Load(const std::string& filepath);//����Shader
        Ref<Shader> Load(const std::string& name, const std::string& filepath);//����shader

        Ref<Shader> Get(const std::string& name);//��ȡshader

        bool Exists(const std::string& name) const;//�ж��Ƿ����shader
    private:
        std::unordered_map<std::string, Ref<Shader>> m_Shaders;//�洢shader��map
    };
}