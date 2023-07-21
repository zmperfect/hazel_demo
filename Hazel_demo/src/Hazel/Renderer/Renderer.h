#pragma once

#include "RenderCommand.h"

#include "OrthographicCamera.h"
#include "Shader.h"

namespace Hazel {

    class Renderer
    {
    public:
        // This function is used to initialize the renderer.
        static void BeginScene(OrthographicCamera& camera);
        static void EndScene();
        // This function is used to submit a shader and a vertex array to the renderer.
        static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

        // This function returns the renderer API.
        inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
    private:
        // This struct is used to store the data of the scene.
        struct SceneData
        {
            glm::mat4 ViewProjectionMatrix;
        };

        static SceneData* s_SceneData;
    };
}
