#pragma once

#include "Hazel/Core/Base.h"

namespace Hazel {

    //帧缓冲区规范
    struct FramebufferSpecification
    {
        uint32_t Width, Height;
        // FramebufferFormat Format = 
        uint32_t Samples = 1;//多重采样

        bool SwapChainTarget = false;//是否是交换链目标
    };

    //帧缓冲区
    class Framebuffer
    {
    public:
        virtual void Bind() = 0;//绑定
        virtual void Unbind() = 0;//解绑

        virtual uint32_t GetColorAttachmentRendererID() const = 0;//获取颜色附件渲染器ID

        virtual const FramebufferSpecification& GetSpecification() const = 0;//获取规范

        static Ref<Framebuffer> Create(const FramebufferSpecification& spec);//根据规范创建帧缓冲区
    };

}