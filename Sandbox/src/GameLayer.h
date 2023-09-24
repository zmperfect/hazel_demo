#pragma once

#include "Hazel.h"

#include "Level.h"
#include <imgui/imgui.h>

//游戏层
class GameLayer : public Hazel::Layer
{
public:
	GameLayer();
	virtual ~GameLayer() = default;

	virtual void OnAttach() override;//将游戏层附加到图层栈
	virtual void OnDetach() override;//将游戏层从图层栈中分离

	void OnUpdate(Hazel::Timestep ts) override;//更新
	virtual void OnImGuiRender() override;//ImGui渲染
	void OnEvent(Hazel::Event& e) override;//事件
	bool OnMouseButtonPressed(Hazel::MouseButtonPressedEvent& e);//鼠标按下事件
	bool OnWindowResize(Hazel::WindowResizeEvent& e);//窗口大小改变事件
private:
	void CreateCamera(uint32_t width, uint32_t height);//创建相机
private:
	Hazel::Scope<Hazel::OrthographicCamera> m_Camera;//相机
	Level m_Level;//关卡
	ImFont* m_Font;//字体
	float m_Time = 0.0f;
	bool m_Blink = false;//闪烁

	//游戏状态
	enum class GameState
	{
		Play = 0, MainMenu = 1, GameOver = 2
	};

	GameState m_State = GameState::MainMenu;//游戏状态，初始为主菜单
};