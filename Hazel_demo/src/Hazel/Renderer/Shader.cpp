#include "hzpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Hazel {

    //����һ��Shader����Ȼ�󷵻�һ��ָ����������ָ��
    Ref<Shader> Shader::Create(const std::string& filepath)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
            case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLShader>(filepath);//�½�һ��OpenGLShader����Ȼ�󷵻�һ��ָ����������ָ��
        }

        HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);//nameҲ����OpenGLShader�Ĺ��캯��
        }

        HZ_CORE_ASSERT(false, "Unknown RenderAPI!");
        return nullptr;
    }

    void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
    {
        HZ_CORE_ASSERT(!Exists(name), "Shader already exists!");//ȷ��û���ظ���Shader
        m_Shaders[name] = shader;//��Shader��ӵ�m_Shaders��
    }

    void ShaderLibrary::Add(const Ref<Shader>& shader)
    {
        auto& name = shader->GetName();//��ȡShader������
        Add(name, shader);//���������Add����
    }

    Hazel::Ref<Hazel::Shader> ShaderLibrary::Load(const std::string& filepath)
    {
        auto shader = Shader::Create(filepath);//����һ��Shader����
        Add(shader);//���������Add����
        return shader;//����һ��ָ����������ָ��
    }

    Hazel::Ref<Hazel::Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
    {
        auto shader = Shader::Create(filepath);//����һ��Shader����
        Add(name, shader);//���������Add����
        return shader;//����һ��ָ����������ָ��
    }

    Hazel::Ref<Hazel::Shader> ShaderLibrary::Get(const std::string& name)
    {
        HZ_CORE_ASSERT(Exists(name), "Shader not found!");//ȷ��Shader����
        return m_Shaders[name];//����name��Ӧ��Shader
    }

    bool ShaderLibrary::Exists(const std::string& name) const
    {
        return m_Shaders.find(name) != m_Shaders.end();//����name��Ӧ��Shader
    }
}
