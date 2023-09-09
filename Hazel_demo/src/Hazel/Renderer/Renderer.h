#pragma once

#include "Hazel/Renderer/RenderCommand.h"

#include "Hazel/Renderer/OrthographicCamera.h"
#include "Hazel/Renderer/Shader.h"

namespace Hazel {

    class Renderer
    {
    public:
        
        static void Init();//初始化
        static void Shutdown();//关闭

        static void OnWindowResize(uint32_t width, uint32_t height);//窗口大小改变

        static void BeginScene(OrthographicCamera& camera);//开始场景
        static void EndScene();//结束场景
        // This function is used to submit a shader and a vertex array to the renderer.
        static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));//提交着色器和顶点数组

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
