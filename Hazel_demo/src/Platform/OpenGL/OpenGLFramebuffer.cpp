#include "hzpch.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"

#include <glad/glad.h>

namespace Hazel {

    static const uint32_t s_MaxFramebufferSize = 8192;//���֡��������С

    namespace Utils {

        static GLenum TextureTarget(bool multisampled)//����Ŀ��
        {
            return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;//����Ƕ��ز�������GL_TEXTURE_2D_MULTISAMPLE�����򷵻�GL_TEXTURE_2D
        }

        static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count)//��������
        {
            glCreateTextures(TextureTarget(multisampled), count, outID);//��������
        }

        static void BindTexture(bool multisampled, uint32_t id)//������
        {
            glBindTexture(TextureTarget(multisampled), id);//������
        }

        static void AttachColorTexture(uint32_t id, int samples, GLenum format, uint32_t width, uint32_t height, int index)//������ɫ����
        {
            bool multisampled = samples > 1;//�Ƿ��Ƕ��ز���
            if (multisampled)//����Ƕ��ز���
            {
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);//�������ز�������
            }
            else
            {
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);//��������
                
                //�����������
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //�������������С���˷�ʽΪ���Թ���
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//����������ķŴ���˷�ʽΪ���Թ���
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);//���д���������������R����ͨ������ȷ��򣩵İ�װ��ʽΪ��Ե�н������������곬��[0,1]��Χʱ���������н�����ӽ��ı�Ե��
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//���д���������������S����ͨ���ǿ�ȷ��򣩵İ�װ��ʽΪ��Ե�н���
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);//���д���������������T����ͨ���Ǹ߶ȷ��򣩵İ�װ��ʽΪ��Ե�н���
            }

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);//�������ӵ�֡������
        }

        static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)//������ɫ����
        {
            bool multisampled = samples > 1;//�Ƿ��Ƕ��ز���
            if (multisampled)//����Ƕ��ز���
            {
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);//�������ز�������
            }
            else
            {
                glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

                //�����������
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //�������������С���˷�ʽΪ���Թ���
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//����������ķŴ���˷�ʽΪ���Թ���
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);//���д���������������R����ͨ������ȷ��򣩵İ�װ��ʽΪ��Ե�н������������곬��[0,1]��Χʱ���������н�����ӽ��ı�Ե��
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//���д���������������S����ͨ���ǿ�ȷ��򣩵İ�װ��ʽΪ��Ե�н���
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);//���д���������������T����ͨ���Ǹ߶ȷ��򣩵İ�װ��ʽΪ��Ե�н���
            }

            glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
        }

        static bool IsDepthFormat(FramebufferTextureFormat format)//�Ƿ�����ȸ�ʽ
        {
            switch (format)
            {
                case FramebufferTextureFormat::DEPTH24STENCIL8:  return true;
            }
            return false;
        }
    }

    OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
        : m_Specification(spec)//ָ��֡�������淶
    {
        for (auto spec : m_Specification.Attachments.Attachments)//�����淶����
        {
            if (!Utils::IsDepthFormat(spec.TextureFormat))//���������ȸ�ʽ
                m_ColorAttachmentSpecifications.emplace_back(spec);//���淶������ӵ���ɫ�����淶
            else
                m_DepthAttachmentSpecification = spec;//���򽫹淶������ӵ���ȸ����淶
        }

        Invalidate();
    }

    OpenGLFramebuffer::~OpenGLFramebuffer()
    {
        glDeleteFramebuffers(1, &m_RendererID);//ɾ����Ⱦ��ID
        glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());//ɾ����ɫ����
        glDeleteTextures(1, &m_DepthAttachment);//ɾ����ȸ���
    }

    void OpenGLFramebuffer::Invalidate()
    {

        if (m_RendererID)
        {
            glDeleteFramebuffers(1, &m_RendererID);//ɾ����Ⱦ��ID
            glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());//ɾ����ɫ����
            glDeleteTextures(1, &m_DepthAttachment);//ɾ����ȸ���

            m_ColorAttachments.clear();//�����ɫ����
            m_DepthAttachment = 0;//��ȸ�����Ϊ0
        }

        glCreateFramebuffers(1, &m_RendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

        bool multisample = m_Specification.Samples > 1;//�Ƿ��Ƕ��ز���

        // Attachments
        if (m_ColorAttachmentSpecifications.size())
        {
            m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
            Utils::CreateTextures(multisample, m_ColorAttachments.data(), m_ColorAttachments.size());

            for (size_t i = 0; i < m_ColorAttachments.size(); i++)
            {
                Utils::BindTexture(multisample, m_ColorAttachments[i]);
                switch (m_ColorAttachmentSpecifications[i].TextureFormat)
                {
                case FramebufferTextureFormat::RGBA8:
                    Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_RGBA8, m_Specification.Width, m_Specification.Height, i);
                    break;
                }
            }
        }

        if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
        {
            Utils::CreateTextures(multisample, &m_DepthAttachment, 1);
            Utils::BindTexture(multisample, m_DepthAttachment);
            switch (m_DepthAttachmentSpecification.TextureFormat)
            {
            case FramebufferTextureFormat::DEPTH24STENCIL8:
                Utils::AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
                break;
            }
        }

        if (m_ColorAttachments.size() > 1)
        {
            HZ_CORE_ASSERT(m_ColorAttachments.size() <= 4);
            GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
            glDrawBuffers(m_ColorAttachments.size(), buffers);
        }
        else if (m_ColorAttachments.empty())
        {
            // Only depth-pass
            glDrawBuffer(GL_NONE);
        }

        HZ_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLFramebuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        glViewport(0, 0, m_Specification.Width, m_Specification.Height);
    }

    void OpenGLFramebuffer::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)//������С
    {
        //������Ч������С
        if (width == 0 || height == 0)
        {
            HZ_CORE_WARN("Attempted to resize framebuffer to {0}, {1}", width, height);
            return;
        }

        m_Specification.Width = width;
        m_Specification.Height = height;

        Invalidate();
    }

}