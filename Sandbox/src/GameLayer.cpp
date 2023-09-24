#include "GameLayer.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace Hazel;

//��Ϸ�㣬��Ϸ����Ҫ�߼�
GameLayer::GameLayer()
	: Layer("GameLayer")
{
	auto& window = Application::Get().GetWindow();//��ȡ����
	CreateCamera(window.GetWidth(), window.GetHeight());//�������

	Random::Init();//��ʼ�������
}

//����Ϸ�㸽�ӵ�ͼ��ջ
void GameLayer::OnAttach()
{
	m_Level.Init();//��ʼ���ؿ�

	ImGuiIO io = ImGui::GetIO();//��ȡImGuiIO
	m_Font = io.Fonts->AddFontFromFileTTF("assets/OpenSans-Regular.ttf", 16.0f);//�������
}

//����Ϸ���ͼ��ջ�з���
void GameLayer::OnDetach()
{
}

//��Ϸ�����
void GameLayer::OnUpdate(Hazel::Timestep ts)
{
	m_Time += ts;//ʱ������

	// Update
	if ((int)(m_Time * 10.0f) % 8 > 4)
		m_Blink = !m_Blink;//��˸

	// ��Ϸ�����ж�
	if (m_Level.IsGameOver())
		m_State = GameState::GameOver;

	//����������
	const auto& playerPos = m_Level.GetPlayer().GetPosition();//��ȡ���λ��
	m_Camera->SetPosition({ playerPos.x, playerPos.y, 0.0f });//�������λ��

	//��Ϸ״̬����
	switch (m_State)
	{
		//����Ϸ״̬ΪPlayʱ�����¹ؿ�
		case GameState::Play:
		{
			m_Level.OnUpdate(ts);//���¹ؿ�
			break;
		}
	}

	// ��Ⱦ
	Hazel::RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1 });//���������ɫ
	Hazel::RenderCommand::Clear();//���

	Hazel::Renderer2D::BeginScene(*m_Camera);//��ʼ��Ⱦ
	m_Level.OnRender();//��Ⱦ�ؿ�
	Hazel::Renderer2D::EndScene();//������Ⱦ
}

void GameLayer::OnImGuiRender()
{
	//�������ڵ��Ե�ImGui���ڣ�����ʱ�򿪣��������涯��������
	ImGui::Begin("Settings");
	m_Level.OnImGuiRender();
	ImGui::End();

	// UI?

	//��ͬ��Ϸ״̬�Ĵ���
	switch (m_State)
	{
		//����Ϸ״̬ΪPlayʱ����ʾ����
		case GameState::Play:
		{
			uint32_t playerScore = m_Level.GetPlayer().GetScore();//��ȡ��ҷ���
			std::string scoreStr = std::string("Score: ") + std::to_string(playerScore);//ת��Ϊ�ַ���
			ImGui::GetForegroundDrawList()->AddText(m_Font, 48.0f, ImGui::GetWindowPos(), 0xffffffff, scoreStr.c_str());//��ʾ����
			break;
		}
		//����Ϸ״̬ΪMainMenuʱ����ʾ��Click to Play!��
		case GameState::MainMenu:
		{
			auto pos = ImGui::GetWindowPos();
			auto width = Application::Get().GetWindow().GetWidth();
			auto height = Application::Get().GetWindow().GetHeight();
			pos.x -= 200.0f;
			pos.y += 50.0f;
			if (m_Blink)
				ImGui::GetForegroundDrawList()->AddText(m_Font, 120.0f, pos, 0xffffffff, "Click to Play!");
			break;
		}
		//����Ϸ״̬ΪGameOverʱ����ʾ��Click to Play!���ͷ���
		case GameState::GameOver:
		{
			auto pos = ImGui::GetWindowPos();
			auto width = Application::Get().GetWindow().GetWidth();
			auto height = Application::Get().GetWindow().GetHeight();
			pos.x -= 200.0f;
			pos.y += 50.0f;
			if (m_Blink)
				ImGui::GetForegroundDrawList()->AddText(m_Font, 120.0f, pos, 0xffffffff, "Click to Play!");

			pos.x += 200.0f;
			pos.y += 150.0f;
			uint32_t playerScore = m_Level.GetPlayer().GetScore();
			std::string scoreStr = std::string("Score: ") + std::to_string(playerScore);
			ImGui::GetForegroundDrawList()->AddText(m_Font, 48.0f, pos, 0xffffffff, scoreStr.c_str());
			break;
		}
	}
}

//�¼�����
void GameLayer::OnEvent(Hazel::Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(GameLayer::OnWindowResize));
	dispatcher.Dispatch<MouseButtonPressedEvent>(HZ_BIND_EVENT_FN(GameLayer::OnMouseButtonPressed));
}

//��갴���¼�����
bool GameLayer::OnMouseButtonPressed(Hazel::MouseButtonPressedEvent& e)
{
	if (m_State == GameState::GameOver)
		m_Level.Reset();

	m_State = GameState::Play;
	return false;
}

//���ڴ�С�ı��¼�����
bool GameLayer::OnWindowResize(Hazel::WindowResizeEvent& e)
{
	CreateCamera(e.GetWidth(), e.GetHeight());
	return false;
}

//�������
void GameLayer::CreateCamera(uint32_t width, uint32_t height)
{
	float aspectRatio = (float)width / (float)height;

	float camWidth = 8.0f;
	float bottom = -camWidth;
	float top = camWidth;
	float left = bottom * aspectRatio;
	float right = top *aspectRatio;
	m_Camera = CreateScope<OrthographicCamera>(left, right, bottom, top);
}
