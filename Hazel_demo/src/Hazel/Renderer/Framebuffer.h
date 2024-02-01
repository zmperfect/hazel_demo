#pragma once

#include "Hazel/Core/Base.h"

namespace Hazel {

    enum class FramebufferTextureFormat//֡�����������ʽ
    {
        None = 0,

        // Color
        RGBA8,//��ͨ��8λ
        RED_INTEGER,//��ɫ����

        // Depth/stencil
        DEPTH24STENCIL8,

        // Defaults
        Depth = DEPTH24STENCIL8
    };

    struct FramebufferTextureSpecification//֡����������淶
    {
        FramebufferTextureSpecification() = default;
        FramebufferTextureSpecification(FramebufferTextureFormat format)
            : TextureFormat(format) {}

        FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
        // TODO: filtering/wrap
    };

    struct FramebufferAttachmentSpecification//֡�����������淶
    {
        FramebufferAttachmentSpecification() = default;
        FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
            : Attachments(attachments) {}

        std::vector<FramebufferTextureSpecification> Attachments;
    };

    //֡�������淶
    struct FramebufferSpecification
    {
        uint32_t Width = 0, Height = 0;
        FramebufferAttachmentSpecification Attachments;//֡�����������淶
        uint32_t Samples = 1;//���ز���

        bool SwapChainTarget = false;//�Ƿ��ǽ�����Ŀ��
    };

    //֡������
    class Framebuffer
    {
    public:
        virtual ~Framebuffer() = default;

        virtual void Bind() = 0;//��
        virtual void Unbind() = 0;//���

        virtual void Resize(uint32_t width, uint32_t height) = 0;//������С
        virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;//��ȡ����

        virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;//�������

        virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;//��ȡ��ɫ������Ⱦ��ID

        virtual const FramebufferSpecification& GetSpecification() const = 0;//��ȡ�淶

        static Ref<Framebuffer> Create(const FramebufferSpecification& spec);//���ݹ淶����֡������
    };

}