#include "Sandbox2D.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
		: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
}

void Sandbox2D::OnAttach()
{
    HZ_PROFILE_FUNCTION();//获取函数签名

    m_CheckerboardTexture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
}

void Sandbox2D::OnDetach()
{
    HZ_PROFILE_FUNCTION();//获取函数签名
}

void Sandbox2D::OnUpdate(Hazel::Timestep ts)
{
    HZ_PROFILE_FUNCTION();//获取函数签名

    // Update
    m_CameraController.OnUpdate(ts);

    // Render
    Hazel::Renderer2D::ResetStats();//重置渲染器统计数据
    {
        HZ_PROFILE_SCOPE("Renderer Prep");
        Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });//{ 0.1f, 0.1f, 0.1f, 1 }指的是RGBA，即红绿蓝透明度，范围是0~1
        Hazel::RenderCommand::Clear();//清除颜色缓冲区和深度缓冲区
    }

    {
        static float rotation = 0.0f;//旋转角度
        rotation += ts * 50.0f;

        HZ_PROFILE_SCOPE("Renderer Draw");
        Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());//开始渲染场景
        Hazel::Renderer2D::DrawRotatedQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, -45.0f, { 0.8f, 0.2f, 0.3f, 1.0f });//绘制旋转矩形，位置，大小，旋转角度，颜色
        Hazel::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
        Hazel::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
        Hazel::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 20.0f, 20.0f }, m_CheckerboardTexture, 10.0f);
        Hazel::Renderer2D::DrawRotatedQuad({ -2.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, rotation, m_CheckerboardTexture, 20.0f);
        Hazel::Renderer2D::EndScene();//结束渲染场景

        Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());//开始渲染场景
        for (float y = -0.5f; y < 5.0f; y += 0.5f)
        {
            for (float x = -0.5f; x < 5.0f; x += 0.5f)
            {
                glm::vec4 color = { (x + 0.5f) / 5.0f, 0.4f, (y + 0.5f) / 5.0f, 0.5f };//颜色
                Hazel::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);//绘制矩形，位置，大小，颜色
            }
        }
        Hazel::Renderer2D::EndScene();//结束渲染场景
    }
}

void Sandbox2D::OnImGuiRender()
{
    HZ_PROFILE_FUNCTION();//获取函数签名

    ImGui::Begin("Settings");//开始设置

    auto stats = Hazel::Renderer2D::GetStats();//获取渲染器统计数据
    //显示渲染器统计数据
    ImGui::Text("Renderer2D Stats:");
    ImGui::Text("Draw Calls: %d", stats.DrawCalls);
    ImGui::Text("Quads: %d", stats.QuadCount);
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));//编辑颜色

    ImGui::End();//结束设置
}

void Sandbox2D::OnEvent(Hazel::Event& e)
{
    m_CameraController.OnEvent(e);
}
