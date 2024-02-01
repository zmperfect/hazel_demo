#pragma once

#include "Hazel/Renderer/Framebuffer.h"

namespace Hazel {

    class OpenGLFramebuffer : public Framebuffer
    {
    public:
        OpenGLFramebuffer(const FramebufferSpecification& spec);
        virtual ~OpenGLFramebuffer();

        void Invalidate();//ʹ��Ч

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual void Resize(uint32_t width, uint32_t height) override;
        virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

        virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;

        virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { HZ_CORE_ASSERT(index < m_ColorAttachments.size()); return m_ColorAttachments[index]; }//����Ƿ񳬹���ɫ���������ޣ�������ɫ��������Ⱦ��ID

        virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }//��ȡ֡�������淶
    private:
        uint32_t m_RendererID = 0;
        FramebufferSpecification m_Specification;

        std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;//��ɫ�����淶
        FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;//��ȸ����淶

        std::vector<uint32_t> m_ColorAttachments;//��ɫ����
        uint32_t m_DepthAttachment = 0;//��ȸ���
    };

}