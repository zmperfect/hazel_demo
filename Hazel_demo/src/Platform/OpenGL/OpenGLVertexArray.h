#pragma once

#include "Hazel/Renderer/VertexArray.h"

namespace Hazel {

    class OpenGLVertexArray : public VertexArray
    {
    public:
        OpenGLVertexArray();
        virtual ~OpenGLVertexArray();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
        virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

        virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; }
        virtual const Ref<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }
    private:
        uint32_t m_RendererID;//��Ⱦ��ID
        uint32_t m_VertexBufferIndex = 0;//���㻺������
        std::vector<Ref<VertexBuffer>> m_VertexBuffers;//���㻺��
        Ref<IndexBuffer> m_IndexBuffer;//��������
    };

}