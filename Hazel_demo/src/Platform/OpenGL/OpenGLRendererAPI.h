#pragma once

#include "Hazel/Renderer/RendererAPI.h"

namespace Hazel {

    class OpenGLRendererAPI : public RendererAPI
    {
    public:
        virtual void Init() override;//��ʼ��
        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;//�����ӿ�

        virtual void SetClearColor(const glm::vec4& color) override;//���������ɫ
        virtual void Clear() override;//���

        virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) override;//���ƶ�������
    };
}