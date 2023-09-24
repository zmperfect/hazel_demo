#include "GameLayer.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace Hazel;

//游戏层，游戏的主要逻辑
GameLayer::GameLayer()
	: Layer("GameLayer")
{
	auto& window = Application::Get().GetWindow();//获取窗口
	CreateCamera(window.GetWidth(), window.GetHeight());//创建相机

	Random::Init();//初始化随机数
}

//将游戏层附加到图层栈
void GameLayer::OnAttach()
{
	m_Level.Init();//初始化关卡

	ImGuiIO io = ImGui::GetIO();//获取ImGuiIO
	m_Font = io.Fonts->AddFontFromFileTTF("assets/OpenSans-Regular.ttf", 16.0f);//添加字体
}

//将游戏层从图层栈中分离
void GameLayer::OnDetach()
{
}

//游戏层更新
void GameLayer::OnUpdate(Hazel::Timestep ts)
{
	m_Time += ts;//时间增加

	// Update
	if ((int)(m_Time * 10.0f) % 8 > 4)
		m_Blink = !m_Blink;//闪烁

	// 游戏结束判断
	if (m_Level.IsGameOver())
		m_State = GameState::GameOver;

	//相机跟随玩家
	const auto& playerPos = m_Level.GetPlayer().GetPosition();//获取玩家位置
	m_Camera->SetPosition({ playerPos.x, playerPos.y, 0.0f });//设置相机位置

	//游戏状态更新
	switch (m_State)
	{
		//当游戏状态为Play时，更新关卡
		case GameState::Play:
		{
			m_Level.OnUpdate(ts);//更新关卡
			break;
		}
	}

	// 渲染
	Hazel::RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1 });//设置清除颜色
	Hazel::RenderCommand::Clear();//清除

	Hazel::Renderer2D::BeginScene(*m_Camera);//开始渲染
	m_Level.OnRender();//渲染关卡
	Hazel::Renderer2D::EndScene();//结束渲染
}

void GameLayer::OnImGuiRender()
{
	//开启用于调试的ImGui窗口，调试时打开，控制引擎动力和重力
	ImGui::Begin("Settings");
	m_Level.OnImGuiRender();
	ImGui::End();

	// UI?

	//不同游戏状态的处理
	switch (m_State)
	{
		//当游戏状态为Play时，显示分数
		case GameState::Play:
		{
			uint32_t playerScore = m_Level.GetPlayer().GetScore();//获取玩家分数
			std::string scoreStr = std::string("Score: ") + std::to_string(playerScore);//转换为字符串
			ImGui::GetForegroundDrawList()->AddText(m_Font, 48.0f, ImGui::GetWindowPos(), 0xffffffff, scoreStr.c_str());//显示分数
			break;
		}
		//当游戏状态为MainMenu时，显示“Click to Play!”
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
		//当游戏状态为GameOver时，显示“Click to Play!”和分数
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

//事件处理
void GameLayer::OnEvent(Hazel::Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(GameLayer::OnWindowResize));
	dispatcher.Dispatch<MouseButtonPressedEvent>(HZ_BIND_EVENT_FN(GameLayer::OnMouseButtonPressed));
}

//鼠标按下事件处理
bool GameLayer::OnMouseButtonPressed(Hazel::MouseButtonPressedEvent& e)
{
	if (m_State == GameState::GameOver)
		m_Level.Reset();

	m_State = GameState::Play;
	return false;
}

//窗口大小改变事件处理
bool GameLayer::OnWindowResize(Hazel::WindowResizeEvent& e)
{
	CreateCamera(e.GetWidth(), e.GetHeight());
	return false;
}

//创建相机
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
