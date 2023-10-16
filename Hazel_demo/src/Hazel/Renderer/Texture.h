#pragma once

#include <string>

#include "Hazel/Core/Base.h"

namespace Hazel {

    class Texture
    {
    public:
        virtual ~Texture() = default;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual uint32_t GetRendererID() const = 0;//获取渲染器ID

        virtual void SetData(void* data, uint32_t size) = 0;//设置纹理数据

        virtual void Bind(uint32_t slot = 0) const = 0;

        virtual bool operator==(const Texture& other) const = 0;//判断两个纹理是否相等
    };

    class Texture2D : public Texture
    {
    public:
        static Ref<Texture2D> Create(uint32_t width, uint32_t height);//创建空白纹理
        static Ref<Texture2D> Create(const std::string& path);//按照路径创建纹理
    };
}