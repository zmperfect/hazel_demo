#include "Sandbox2D.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>

template<typename Fn>
//���غ���ִ�е�ʱ��
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

        long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();//ʱ��ת��
        long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

        m_Stopped = true;

        float duration = (end - start) * 0.001f;//����ʱ��ת��Ϊ����
        m_Func({ m_Name, duration });
    }

private:
    const char* m_Name;//������
    Fn m_Func;//����
    std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;//��ʼʱ��
    bool m_Stopped;//ֹͣ��־
};

#define PROFILE_SCOPE(name) Timer timer##__LINE__(name, [&](ProfileResult profileResult) { m_ProfileResults.push_back(profileResult); })//����PROFILE_SCOPE�꣬������name����������,Ȼ�����Timer�࣬��name��һ��lambda������Ϊ��������

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
        Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });//{ 0.1f, 0.1f, 0.1f, 1 }ָ����RGBA����������͸���ȣ���Χ��0~1
        Hazel::RenderCommand::Clear();//�����ɫ����������Ȼ�����
    }

    {
        PROFILE_SCOPE("Renderer Draw");
        Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());//��ʼ��Ⱦ����
        Hazel::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });//���ƾ��Σ������ֱ���λ�á���С����ɫ
        Hazel::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
        Hazel::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, m_CheckerboardTexture);//���Ʊ���
        Hazel::Renderer2D::EndScene();//������Ⱦ����
    }
}

void Sandbox2D::OnImGuiRender()
{
    ImGui::Begin("Settings");//��ʼ����
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));//�༭��ɫ

    //���ܲ���
    for (auto& result : m_ProfileResults)//�������Խ��
    {
        char label[50];//��ǩ
        strcpy(label, "%.3fms ");//�����ַ���
        strcat(label, result.Name);//�����ַ���
        ImGui::Text(label, result.Time);//��ʾ��ǩ��ʱ��
    }
    m_ProfileResults.clear();

    ImGui::End();//��������
}

void Sandbox2D::OnEvent(Hazel::Event& e)
{
    m_CameraController.OnEvent(e);
}
