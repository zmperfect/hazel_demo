#include "hzpch.h"
#include "Hazel/Renderer/Shader.h"

#include "Hazel/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Hazel {

    //创建一个Shader对象，然后返回一个指向它的智能指针
    Ref<Shader> Shader::Create(const std::string& filepath)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
            case RendererAPI::API::OpenGL:  return CreateScope<OpenGLShader>(filepath);//新建一个OpenGLShader对象，然后返回一个指向它的智能指针
        }

        HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:  return CreateScope<OpenGLShader>(name, vertexSrc, fragmentSrc);//name也传给OpenGLShader的构造函数
        }

        HZ_CORE_ASSERT(false, "Unknown RenderAPI!");
        return nullptr;
    }

    void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
    {
        HZ_CORE_ASSERT(!Exists(name), "Shader already exists!");//确保没有重复的Shader
        m_Shaders[name] = shader;//将Shader添加到m_Shaders中
    }

    void ShaderLibrary::Add(const Ref<Shader>& shader)
    {
        auto& name = shader->GetName();//获取Shader的名字
        Add(name, shader);//调用上面的Add函数
    }

    Ref<Shader> ShaderLibrary::Load(const std::string& filepath)
    {
        auto shader = Shader::Create(filepath);//创建一个Shader对象
        Add(shader);//调用上面的Add函数
        return shader;//返回一个指向它的智能指针
    }

    Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
    {
        auto shader = Shader::Create(filepath);//创建一个Shader对象
        Add(name, shader);//调用上面的Add函数
        return shader;//返回一个指向它的智能指针
    }

    Ref<Shader> ShaderLibrary::Get(const std::string& name)
    {
        HZ_CORE_ASSERT(Exists(name), "Shader not found!");//确保Shader存在
        return m_Shaders[name];//返回name对应的Shader
    }

    bool ShaderLibrary::Exists(const std::string& name) const
    {
        return m_Shaders.find(name) != m_Shaders.end();//查找name对应的Shader
    }
}
