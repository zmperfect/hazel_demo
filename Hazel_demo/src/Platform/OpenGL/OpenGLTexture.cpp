#include "hzpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"

#include <glad/glad.h>

namespace Hazel {

    OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
        :m_Path(path)
    {
        int width, height, channels;//���ߣ�ͨ����
        stbi_set_flip_vertically_on_load(1);//��תͼƬ
        stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);//����ͼƬ
        HZ_CORE_ASSERT(data, "Failed to load image!");
        m_Width = width;
        m_Height = height;

        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);//��������
        glTextureStorage2D(m_RendererID, 1, GL_RGBA8, m_Width, m_Height);//��������洢�ռ�

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//����������С������
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//������󻯲���

        //glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);//�����ظ�����

        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, GL_RGB, GL_UNSIGNED_BYTE, data);//�����������ݣ������½ǿ�ʼ�����Ϊm_Width,m_Height����ʽΪRGB������Ϊ�޷����ֽڣ�����Ϊdata

        stbi_image_free(data);//�ͷ�ͼƬ����
    }

    OpenGLTexture2D::~OpenGLTexture2D()
    {
        glDeleteTextures(1, &m_RendererID);//ɾ������
    }

    void OpenGLTexture2D::Bind(uint32_t slot) const
    {
        glBindTextureUnit(slot, m_RendererID);//������
    }
}