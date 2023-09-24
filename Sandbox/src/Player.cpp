#include "Player.h"

#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>

using namespace Hazel;

Player::Player()
{
	// 尾气
	m_SmokeParticle.Position = { 0.0f, 0.0f };//位置
	m_SmokeParticle.Velocity = { -2.0f, 0.0f }, m_SmokeParticle.VelocityVariation = { 4.0f, 2.0f };//速度，速度变化，用于模拟随机性，使粒子看起来更自然，第一个参数是x轴速度，第二个参数是y轴速度
	m_SmokeParticle.SizeBegin = 0.35f, m_SmokeParticle.SizeEnd = 0.0f, m_SmokeParticle.SizeVariation = 0.15f;//粒子大小，大小变化，用于模拟随机性，使粒子看起来更自然
	m_SmokeParticle.ColorBegin = { 0.8f, 0.8f, 0.8f, 1.0f };//初始粒子颜色，RGBA
	m_SmokeParticle.ColorEnd = { 0.6f, 0.6f, 0.6f, 1.0f };//末端粒子颜色，RGBA
	m_SmokeParticle.LifeTime = 4.0f;//粒子生命周期

	// 引擎喷射火焰
	m_EngineParticle.Position = { 0.0f, 0.0f };
	m_EngineParticle.Velocity = { -2.0f, 0.0f }, m_EngineParticle.VelocityVariation = { 3.0f, 1.0f };
	m_EngineParticle.SizeBegin = 0.5f, m_EngineParticle.SizeEnd = 0.0f, m_EngineParticle.SizeVariation = 0.3f;
	m_EngineParticle.ColorBegin = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	m_EngineParticle.ColorEnd = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f , 1.0f };
	m_EngineParticle.LifeTime = 1.0f;
}

//加载资源
void Player::LoadAssets()
{
	m_ShipTexture = Texture2D::Create("assets/textures/Ship.png");//加载ship纹理
}

//更新
void Player::OnUpdate(Hazel::Timestep ts)
{
	m_Time += ts;

	//通过空格键控制ship上升
	if (Input::IsKeyPressed(HZ_KEY_SPACE))
	{
		m_Velocity.y += m_EnginePower;//ship速度增加一个引擎动力

		// 使ship上升时，引擎喷射火焰更大
		if (m_Velocity.y < 0.0f)
			m_Velocity.y += m_EnginePower * 2.0f;//ship速度增加两倍的引擎动力

		// 引擎喷射火焰
		glm::vec2 emissionPoint = { 0.0f, -0.6f };//发射点
		float rotation = glm::radians(GetRotation());//旋转角度
		glm::vec4 rotated = glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f }) * glm::vec4(emissionPoint, 0.0f, 1.0f);//旋转后的发射点，用于计算粒子的位置，旋转矩阵*发射点
		m_EngineParticle.Position = m_Position + glm::vec2{ rotated.x, rotated.y };//粒子位置
		m_EngineParticle.Velocity.y = -m_Velocity.y * 0.2f - 0.2f;//粒子速度，与ship飞行方向相反
		m_ParticleSystem.Emit(m_EngineParticle);//发射粒子
	}
	else
	{
		m_Velocity.y -= m_Gravity;//，不启用引擎，则ship只受重力影响
	}

	m_Velocity.y = glm::clamp(m_Velocity.y, -20.0f, 20.0f);//限制ship速度（y轴方向）
	m_Position += m_Velocity * (float)ts;//更新ship位置

	// 粒子系统
	if (m_Time > m_SmokeNextEmitTime)//每隔一段时间发射一次粒子
	{
		m_SmokeParticle.Position = m_Position;//粒子位置
		m_ParticleSystem.Emit(m_SmokeParticle);//发射粒子
		m_SmokeNextEmitTime += m_SmokeEmitInterval;//下次发射时间
	}

	m_ParticleSystem.OnUpdate(ts);//更新粒子系统
}

//渲染
void Player::OnRender()
{
	m_ParticleSystem.OnRender();//渲染粒子系统
	Renderer2D::DrawQuad({ m_Position.x, m_Position.y, 0.5f }, { 1.0f, 1.3f }, glm::radians(GetRotation()), m_ShipTexture);//渲染ship
}

//ImGui渲染
void Player::OnImGuiRender()
{
	ImGui::DragFloat("Engine Power", &m_EnginePower, 0.1f);//拖拽条，用于调整引擎动力
	ImGui::DragFloat("Gravity", &m_Gravity, 0.1f);//拖拽条，用于调整重力
}

//重置
void Player::Reset()
{
	m_Position = { -10.0f, 0.0f };
	m_Velocity = { 5.0f, 0.0f };
}
