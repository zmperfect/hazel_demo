#pragma once

#include"Hazel/Renderer/RendererAPI.h"

namespace Hazel {

    class RenderCommand
    {
    public:
        static void Init()
        {
            s_RendererAPI->Init();//��ʼ����Ⱦ��API
        }

        static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
        {
            s_RendererAPI->SetViewport(x, y, width, height);//�����ӿ�
        }

        static void SetClearColor(const glm::vec4& color)
        {
            s_RendererAPI->SetClearColor(color);
        }

        static void Clear()
        {
            s_RendererAPI->Clear();
        }

        // ����������
        static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexcount = 0)
        {
            s_RendererAPI->DrawIndexed(vertexArray, indexcount);
        }

        // ����
        static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
        {
            s_RendererAPI->DrawLines(vertexArray, vertexCount);
        }

        // �����߿�
        static void SetLineWidth(float width)
        {
            s_RendererAPI->SetLineWidth(width);
        }
    private:
        static Scope<RendererAPI> s_RendererAPI;
    };
}