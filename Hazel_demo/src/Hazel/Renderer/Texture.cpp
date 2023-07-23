#include "hzpch.h"
#include "Texture.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Hazel {

    //��ά������
    Ref<Texture2D> Texture2D::Create(const std::string& path)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLTexture2D>(path);//����һ��OpenGL�������
        }

        HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}