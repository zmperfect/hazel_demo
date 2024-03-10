#pragma once

#include "Hazel/Renderer/Texture.h"

#include <glad/glad.h>

namespace Hazel {

    class OpenGLTexture2D : public Texture2D
    {
    public:
        OpenGLTexture2D(uint32_t width, uint32_t height);//按照宽高创建纹理
        OpenGLTexture2D(const std::string& path);//按照路径加载纹理
        virtual ~OpenGLTexture2D();

        virtual uint32_t GetWidth() const override { return m_Width; }
        virtual uint32_t GetHeight() const override { return m_Height; }
        virtual uint32_t GetRendererID() const override { return m_RendererID; }

        virtual void SetData(void* data, uint32_t size) override;//设置纹理数据

        virtual void Bind(uint32_t slot = 0) const override;//绑定纹理

        virtual bool IsLoaded() const override { return m_RendererID; }//判断纹理是否加载

        virtual bool operator==(const Texture& other) const override
        {
            return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
        }
    private:
        std::string m_Path;
        bool m_IsLoaded = false;
        uint32_t m_Width, m_Height;
        uint32_t m_RendererID;//OpenGL texture id
        GLenum m_InternalFormat, m_DataFormat;//纹理的内部格式和数据格式
    };
}