#pragma once

#include <glm/glm.hpp>

#include "Hazel/Renderer/VertexArray.h"

namespace Hazel {

    class RendererAPI
    {
    public:
        enum class API
        {
            None = 0, OpenGL = 1
        };
    public:
        virtual void Init() = 0;//��ʼ��
        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;//�����ӿ�
        virtual void SetClearColor(const glm::vec4& color) = 0;//���������ɫ
        virtual void Clear() = 0;//���

        virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t count = 0) = 0;//���ƶ�������

        static API GetAPI() { return s_API; }//��ȡAPI
        static Scope<RendererAPI> Create();//����

    private:
        static API s_API;//API
    };
}