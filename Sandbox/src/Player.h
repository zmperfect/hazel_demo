#pragma once

#include <Hazel.h>

#include "Color.h"
#include "Random.h"

#include "ParticleSystem.h"

class Player
{
public:
	Player();

	void LoadAssets();//加载资源

	void OnUpdate(Hazel::Timestep ts);//更新
	void OnRender();//渲染

	void OnImGuiRender();//ImGui渲染

	void Reset();//重置

	float GetRotation() { return m_Velocity.y * 4.0f - 90.0f; }//获取角度，和ship当前速度相关
	const glm::vec2& GetPosition() const { return m_Position; }

	uint32_t GetScore() const { return (uint32_t)(m_Position.x + 10.0f) / 10.0f; }//得分，依据ship飞行距离来计算
private:
	glm::vec2 m_Position = { -10.0f, 0.0f };//位置
	glm::vec2 m_Velocity = { 5.0f, 0.0f };//速度

	float m_EnginePower = 0.5f;//引擎动力
	float m_Gravity = 0.4f;//重力，使ship自由掉落

	float m_Time = 0.0f;//时间
	float m_SmokeEmitInterval = 0.4f;//尾气粒子发射间隔
	float m_SmokeNextEmitTime = m_SmokeEmitInterval;//尾气粒子发射间隔
	 
	ParticleProps m_SmokeParticle, m_EngineParticle;//粒子属性
	ParticleSystem m_ParticleSystem;//粒子系统

	Hazel::Ref<Hazel::Texture2D> m_ShipTexture;//ship纹理
};