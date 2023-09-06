#include "hzpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"

namespace Hazel {

    OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
        :m_Width(width), m_Height(height)
    {
        m_InternalFormat = GL_RGBA8;//内部格式为RGBA8
        m_DataFormat = GL_RGBA;//数据格式为RGBA

        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);//创建纹理
        glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);//创建纹理存储空间

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//设置纹理最小化参数
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//设置最大化参数

        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);//设置水平重复
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);//设置垂直重复
    }

    OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
        :m_Path(path)
    {
        int width, height, channels;//宽，高，通道数
        stbi_set_flip_vertically_on_load(1);//翻转图片
        stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);//加载图片
        HZ_CORE_ASSERT(data, "Failed to load image!");
        m_Width = width;
        m_Height = height;

        GLenum internalFormat = 0, dataFormat = 0;//内部格式，数据格式
        if (channels == 4)//通道数为4
        {
            internalFormat = GL_RGBA8;//内部格式为RGBA8
            dataFormat = GL_RGBA;//数据格式为RGBA
        }
        else if (channels == 3)//通道数为3
        {
            internalFormat = GL_RGB8;//内部格式为RGB8
            dataFormat = GL_RGB;//数据格式为RGB
        }

        m_InternalFormat = internalFormat;
        m_DataFormat = dataFormat;

        HZ_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);//创建纹理
        glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);//创建纹理存储空间

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//设置纹理最小化参数
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//设置最大化参数

        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);//设置水平重复
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);//设置垂直重复

        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);//设置纹理数据，从左下角开始，宽高为m_Width,m_Height，格式为RGB，类型为无符号字节，数据为data

        stbi_image_free(data);//释放图片数据
    }

    OpenGLTexture2D::~OpenGLTexture2D()
    {
        glDeleteTextures(1, &m_RendererID);//删除纹理
    }

    void OpenGLTexture2D::SetData(void* data, uint32_t size)
    {
        uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;//当 m_DataFormat 的值为 GL_RGBA 时，每个像素需要4个字节（RGB颜色通道以及一个Alpha通道），所以 bpp 被设置为 4；当 m_DataFormat 的值不为 GL_RGBA 时，每个像素只需要3个字节（仅有RGB颜色通道，没有Alpha通道），所以 bpp 被设置为 3
        HZ_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");//判断数据大小是否正确
        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);//设置纹理数据，从左下角开始，宽高为m_Width,m_Height，格式为m_DataFormat，类型为无符号字节，数据为data
    }

    void OpenGLTexture2D::Bind(uint32_t slot) const
    {
        glBindTextureUnit(slot, m_RendererID);//绑定纹理
    }
}