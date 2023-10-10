#pragma once

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

namespace Hazel {

    class Shader
    {
    public:
        virtual ~Shader() = default;//使用默认的析构函数

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void SetInt(const std::string& name, int value) = 0;//设置int值
        virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;//设置int数组
        virtual void SetFloat(const std::string& name, float value) = 0;
        virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;//设置float3值
        virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;//设置float4值
        virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;//设置mat4值

        virtual const std::string& GetName() const = 0;//获取shader的名字

        static Ref<Shader> Create(const std::string& filepath);//根据文件路径创建shader
        static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);//根据字符串创建shader
    };

    class ShaderLibrary
    {
    public:
        void Add(const std::string& name, const Ref<Shader>& shader);//添加shader
        void Add(const Ref<Shader>& shader);//添加shader
        Ref<Shader> Load(const std::string& filepath);//加载Shader
        Ref<Shader> Load(const std::string& name, const std::string& filepath);//加载shader

        Ref<Shader> Get(const std::string& name);//获取shader

        bool Exists(const std::string& name) const;//判断是否存在shader
    private:
        std::unordered_map<std::string, Ref<Shader>> m_Shaders;//存储shader的map
    };
}