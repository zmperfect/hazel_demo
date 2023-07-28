#pragma once

#include <string>
#include <unordered_map>

namespace Hazel {

    class Shader
    {
    public:
        virtual ~Shader() = default;//使用默认的析构函数

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

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