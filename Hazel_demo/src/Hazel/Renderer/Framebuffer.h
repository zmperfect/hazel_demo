#pragma once

#include "Hazel/Core/Base.h"

namespace Hazel {

    //֡�������淶
    struct FramebufferSpecification
    {
        uint32_t Width, Height;
        // FramebufferFormat Format = 
        uint32_t Samples = 1;//���ز���

        bool SwapChainTarget = false;//�Ƿ��ǽ�����Ŀ��
    };

    //֡������
    class Framebuffer
    {
    public:
        virtual void Bind() = 0;//��
        virtual void Unbind() = 0;//���

        virtual uint32_t GetColorAttachmentRendererID() const = 0;//��ȡ��ɫ������Ⱦ��ID

        virtual const FramebufferSpecification& GetSpecification() const = 0;//��ȡ�淶

        static Ref<Framebuffer> Create(const FramebufferSpecification& spec);//���ݹ淶����֡������
    };

}