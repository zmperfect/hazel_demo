#pragma once

#include "Hazel/Renderer/RenderCommand.h"

#include "Hazel/Renderer/OrthographicCamera.h"
#include "Hazel/Renderer/Shader.h"

namespace Hazel {

    class Renderer
    {
    public:
        
        static void Init();//��ʼ��
        static void Shutdown();//�ر�

        static void OnWindowResize(uint32_t width, uint32_t height);//���ڴ�С�ı�

        static void BeginScene(OrthographicCamera& camera);//��ʼ����
        static void EndScene();//��������
        // This function is used to submit a shader and a vertex array to the renderer.
        static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));//�ύ��ɫ���Ͷ�������

        // This function returns the renderer API.
        inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
    private:
        // This struct is used to store the data of the scene.
        struct SceneData
        {
            glm::mat4 ViewProjectionMatrix;
        };

        static Scope<SceneData> s_SceneData;
    };
}
