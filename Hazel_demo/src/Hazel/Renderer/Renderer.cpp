#include "hzpch.h"
#include "Hazel/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Hazel/Renderer/Renderer2D.h"

namespace Hazel {

    Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();//创建一个场景数据的作用域

    void Renderer::Init()
    {
        HZ_PROFILE_FUNCTION();//获取函数签名

        RenderCommand::Init();//初始化渲染命令
        Renderer2D::Init();//初始化2D渲染器
    }

    void Renderer::Shutdown()
    {
        Renderer2D::Shutdown();//关闭2D渲染器
    }

    void Renderer::OnWindowResize(uint32_t width, uint32_t height)
    {
        RenderCommand::SetViewport(0, 0, width, height);//设置视口，x,y 以像素为单位，指定了视口的左下角位置。width，height 表示这个视口矩形的宽度和高度，根据窗口的实时变化重绘窗口。
    }

    void Renderer::BeginScene(OrthographicCamera& camera)
    {
        s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();//获取视角投影矩阵
    }

    void Renderer::EndScene()
    {
    }

    void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
    {
        shader->Bind();//绑定着色器
        shader->SetMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);//上传视角投影矩阵
        shader->SetMat4("u_Transform", transform);//上传变换矩阵

        vertexArray->Bind();//绑定顶点数组
        RenderCommand::DrawIndexed(vertexArray);//绘制顶点数组
    }
}
