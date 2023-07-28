#pragma once

#include <string>
#include <unordered_map>

namespace Hazel {

    class Shader
    {
    public:
        virtual ~Shader() = default;//ʹ��Ĭ�ϵ���������

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

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