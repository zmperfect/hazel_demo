#include "hzpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"

namespace Hazel {

    OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
        :m_Width(width), m_Height(height)
    {
        m_InternalFormat = GL_RGBA8;//�ڲ���ʽΪRGBA8
        m_DataFormat = GL_RGBA;//���ݸ�ʽΪRGBA

        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);//��������
        glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);//��������洢�ռ�

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//����������С������
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//������󻯲���

        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);//����ˮƽ�ظ�
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);//���ô�ֱ�ظ�
    }

    OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
        :m_Path(path)
    {
        int width, height, channels;//���ߣ�ͨ����
        stbi_set_flip_vertically_on_load(1);//��תͼƬ
        stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);//����ͼƬ
        HZ_CORE_ASSERT(data, "Failed to load image!");
        m_Width = width;
        m_Height = height;

        GLenum internalFormat = 0, dataFormat = 0;//�ڲ���ʽ�����ݸ�ʽ
        if (channels == 4)//ͨ����Ϊ4
        {
            internalFormat = GL_RGBA8;//�ڲ���ʽΪRGBA8
            dataFormat = GL_RGBA;//���ݸ�ʽΪRGBA
        }
        else if (channels == 3)//ͨ����Ϊ3
        {
            internalFormat = GL_RGB8;//�ڲ���ʽΪRGB8
            dataFormat = GL_RGB;//���ݸ�ʽΪRGB
        }

        m_InternalFormat = internalFormat;
        m_DataFormat = dataFormat;

        HZ_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);//��������
        glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);//��������洢�ռ�

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//����������С������
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//������󻯲���

        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);//����ˮƽ�ظ�
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);//���ô�ֱ�ظ�

        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);//�����������ݣ������½ǿ�ʼ�����Ϊm_Width,m_Height����ʽΪRGB������Ϊ�޷����ֽڣ�����Ϊdata

        stbi_image_free(data);//�ͷ�ͼƬ����
    }

    OpenGLTexture2D::~OpenGLTexture2D()
    {
        glDeleteTextures(1, &m_RendererID);//ɾ������
    }

    void OpenGLTexture2D::SetData(void* data, uint32_t size)
    {
        uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;//�� m_DataFormat ��ֵΪ GL_RGBA ʱ��ÿ��������Ҫ4���ֽڣ�RGB��ɫͨ���Լ�һ��Alphaͨ���������� bpp ������Ϊ 4���� m_DataFormat ��ֵ��Ϊ GL_RGBA ʱ��ÿ������ֻ��Ҫ3���ֽڣ�����RGB��ɫͨ����û��Alphaͨ���������� bpp ������Ϊ 3
        HZ_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");//�ж����ݴ�С�Ƿ���ȷ
        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);//�����������ݣ������½ǿ�ʼ�����Ϊm_Width,m_Height����ʽΪm_DataFormat������Ϊ�޷����ֽڣ�����Ϊdata
    }

    void OpenGLTexture2D::Bind(uint32_t slot) const
    {
        glBindTextureUnit(slot, m_RendererID);//������
    }
}