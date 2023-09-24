#pragma once

#include "Hazel.h"

#include "Level.h"
#include <imgui/imgui.h>

//��Ϸ��
class GameLayer : public Hazel::Layer
{
public:
	GameLayer();
	virtual ~GameLayer() = default;

	virtual void OnAttach() override;//����Ϸ�㸽�ӵ�ͼ��ջ
	virtual void OnDetach() override;//����Ϸ���ͼ��ջ�з���

	void OnUpdate(Hazel::Timestep ts) override;//����
	virtual void OnImGuiRender() override;//ImGui��Ⱦ
	void OnEvent(Hazel::Event& e) override;//�¼�
	bool OnMouseButtonPressed(Hazel::MouseButtonPressedEvent& e);//��갴���¼�
	bool OnWindowResize(Hazel::WindowResizeEvent& e);//���ڴ�С�ı��¼�
private:
	void CreateCamera(uint32_t width, uint32_t height);//�������
private:
	Hazel::Scope<Hazel::OrthographicCamera> m_Camera;//���
	Level m_Level;//�ؿ�
	ImFont* m_Font;//����
	float m_Time = 0.0f;
	bool m_Blink = false;//��˸

	//��Ϸ״̬
	enum class GameState
	{
		Play = 0, MainMenu = 1, GameOver = 2
	};

	GameState m_State = GameState::MainMenu;//��Ϸ״̬����ʼΪ���˵�
};