#include "Sandbox2D.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>

template<typename Fn>
//返回函数执行的时间
class Timer
{
public:
    Timer(const char* name, Fn&& func)
        : m_Name(name), m_Func(func), m_Stopped(false)
    {
        m_StartTimepoint = std::chrono::high_resolution_clock::now();
    }

    ~Timer()
    {
        if (!m_Stopped)
            Stop();
    }

    void Stop()
    {
        auto endTimepoint = std::chrono::high_resolution_clock::now();

        long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();//时间转换
        long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

        m_Stopped = true;

        float duration = (end - start) * 0.001f;//持续时间转换为毫秒
        m_Func({ m_Name, duration });
    }

private:
    const char* m_Name;//函数名
    Fn m_Func;//函数
    std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;//开始时间
    bool m_Stopped;//停止标志
};

#define PROFILE_SCOPE(name) Timer timer##__LINE__(name, [&](ProfileResult profileResult) { m_ProfileResults.push_back(profileResult); })//定义PROFILE_SCOPE宏，参数是name，即函数名,然后调用Timer类，将name和一个lambda函数作为参数传入

Sandbox2D::Sandbox2D()
		: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
}

void Sandbox2D::OnAttach()
{
    m_CheckerboardTexture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(Hazel::Timestep ts)
{
    PROFILE_SCOPE("Sandbox2D::OnUpdate");

    // Update
    {
        PROFILE_SCOPE("CameraController::OnUpdate");
        m_CameraController.OnUpdate(ts);
    }

    // Render
    {
        PROFILE_SCOPE("Renderer Prep");
        Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });//{ 0.1f, 0.1f, 0.1f, 1 }指的是RGBA，即红绿蓝透明度，范围是0~1
        Hazel::RenderCommand::Clear();//清除颜色缓冲区和深度缓冲区
    }

    {
        PROFILE_SCOPE("Renderer Draw");
        Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());//开始渲染场景
        Hazel::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });//绘制矩形，参数分别是位置、大小、颜色
        Hazel::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
        Hazel::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, m_CheckerboardTexture);//绘制背景
        Hazel::Renderer2D::EndScene();//结束渲染场景
    }
}

void Sandbox2D::OnImGuiRender()
{
    ImGui::Begin("Settings");//开始设置
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));//编辑颜色

    //性能测试
    for (auto& result : m_ProfileResults)//遍历测试结果
    {
        char label[50];//标签
        strcpy(label, "%.3fms ");//复制字符串
        strcat(label, result.Name);//连接字符串
        ImGui::Text(label, result.Time);//显示标签和时间
    }
    m_ProfileResults.clear();

    ImGui::End();//结束设置
}

void Sandbox2D::OnEvent(Hazel::Event& e)
{
    m_CameraController.OnEvent(e);
}
