#include "hzpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"

#include <glad/glad.h>

namespace Hazel {

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

        HZ_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);//创建纹理
        glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);//创建纹理存储空间

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//设置纹理最小化参数
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//设置最大化参数

        //glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);//设置重复参数

        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);//设置纹理数据，从左下角开始，宽高为m_Width,m_Height，格式为RGB，类型为无符号字节，数据为data

        stbi_image_free(data);//释放图片数据
    }

    OpenGLTexture2D::~OpenGLTexture2D()
    {
        glDeleteTextures(1, &m_RendererID);//删除纹理
    }

    void OpenGLTexture2D::Bind(uint32_t slot) const
    {
        glBindTextureUnit(slot, m_RendererID);//绑定纹理
    }
}