#include "hzpch.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"

#include <glad/glad.h>

namespace Hazel {

    static const uint32_t s_MaxFramebufferSize = 8192;//最大帧缓冲区大小

    namespace Utils {

        static GLenum TextureTarget(bool multisampled)//纹理目标
        {
            return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;//如果是多重采样返回GL_TEXTURE_2D_MULTISAMPLE，否则返回GL_TEXTURE_2D
        }

        static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count)//创建纹理
        {
            glCreateTextures(TextureTarget(multisampled), count, outID);//创建纹理
        }

        static void BindTexture(bool multisampled, uint32_t id)//绑定纹理
        {
            glBindTexture(TextureTarget(multisampled), id);//绑定纹理
        }

        static void AttachColorTexture(uint32_t id, int samples, GLenum format, uint32_t width, uint32_t height, int index)//附加颜色纹理
        {
            bool multisampled = samples > 1;//是否是多重采样
            if (multisampled)//如果是多重采样
            {
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);//创建多重采样纹理
            }
            else
            {
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);//创建纹理
                
                //设置纹理参数
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //设置了纹理的缩小过滤方式为线性过滤
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//设置了纹理的放大过滤方式为线性过滤
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);//这行代码设置了纹理在R方向（通常是深度方向）的包装方式为边缘夹紧。当纹理坐标超出[0,1]范围时，它将被夹紧到最接近的边缘。
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//这行代码设置了纹理在S方向（通常是宽度方向）的包装方式为边缘夹紧。
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);//这行代码设置了纹理在T方向（通常是高度方向）的包装方式为边缘夹紧。
            }

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);//将纹理附加到帧缓冲区
        }

        static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)//附加颜色纹理
        {
            bool multisampled = samples > 1;//是否是多重采样
            if (multisampled)//如果是多重采样
            {
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);//创建多重采样纹理
            }
            else
            {
                glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

                //设置纹理参数
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //设置了纹理的缩小过滤方式为线性过滤
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//设置了纹理的放大过滤方式为线性过滤
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);//这行代码设置了纹理在R方向（通常是深度方向）的包装方式为边缘夹紧。当纹理坐标超出[0,1]范围时，它将被夹紧到最接近的边缘。
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//这行代码设置了纹理在S方向（通常是宽度方向）的包装方式为边缘夹紧。
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);//这行代码设置了纹理在T方向（通常是高度方向）的包装方式为边缘夹紧。
            }

            glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
        }

        static bool IsDepthFormat(FramebufferTextureFormat format)//是否是深度格式
        {
            switch (format)
            {
                case FramebufferTextureFormat::DEPTH24STENCIL8:  return true;
            }
            return false;
        }
    }

    OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
        : m_Specification(spec)//指定帧缓冲区规范
    {
        for (auto spec : m_Specification.Attachments.Attachments)//遍历规范附件
        {
            if (!Utils::IsDepthFormat(spec.TextureFormat))//如果不是深度格式
                m_ColorAttachmentSpecifications.emplace_back(spec);//将规范附件添加到颜色附件规范
            else
                m_DepthAttachmentSpecification = spec;//否则将规范附件添加到深度附件规范
        }

        Invalidate();
    }

    OpenGLFramebuffer::~OpenGLFramebuffer()
    {
        glDeleteFramebuffers(1, &m_RendererID);//删除渲染器ID
        glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());//删除颜色附件
        glDeleteTextures(1, &m_DepthAttachment);//删除深度附件
    }

    void OpenGLFramebuffer::Invalidate()
    {

        if (m_RendererID)
        {
            glDeleteFramebuffers(1, &m_RendererID);//删除渲染器ID
            glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());//删除颜色附件
            glDeleteTextures(1, &m_DepthAttachment);//删除深度附件

            m_ColorAttachments.clear();//清除颜色附件
            m_DepthAttachment = 0;//深度附件置为0
        }

        glCreateFramebuffers(1, &m_RendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

        bool multisample = m_Specification.Samples > 1;//是否是多重采样

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

    void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)//调整大小
    {
        //避免无效调整大小
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