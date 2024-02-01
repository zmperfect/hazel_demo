#pragma once

#include "Hazel/Renderer/Framebuffer.h"

namespace Hazel {

    class OpenGLFramebuffer : public Framebuffer
    {
    public:
        OpenGLFramebuffer(const FramebufferSpecification& spec);
        virtual ~OpenGLFramebuffer();

        void Invalidate();//使无效

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual void Resize(uint32_t width, uint32_t height) override;
        virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

        virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;

        virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { HZ_CORE_ASSERT(index < m_ColorAttachments.size()); return m_ColorAttachments[index]; }//检查是否超过颜色附件的上限，返回颜色附件的渲染器ID

        virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }//获取帧缓冲区规范
    private:
        uint32_t m_RendererID = 0;
        FramebufferSpecification m_Specification;

        std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;//颜色附件规范
        FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;//深度附件规范

        std::vector<uint32_t> m_ColorAttachments;//颜色附件
        uint32_t m_DepthAttachment = 0;//深度附件
    };

}