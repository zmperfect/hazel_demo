#pragma once

#include <Hazel.h>

#include "Color.h"
#include "Random.h"

#include "ParticleSystem.h"

class Player
{
public:
	Player();

	void LoadAssets();//������Դ

	void OnUpdate(Hazel::Timestep ts);//����
	void OnRender();//��Ⱦ

	void OnImGuiRender();//ImGui��Ⱦ

	void Reset();//����

	float GetRotation() { return m_Velocity.y * 4.0f - 90.0f; }//��ȡ�Ƕȣ���ship��ǰ�ٶ����
	const glm::vec2& GetPosition() const { return m_Position; }

	uint32_t GetScore() const { return (uint32_t)(m_Position.x + 10.0f) / 10.0f; }//�÷֣�����ship���о���������
private:
	glm::vec2 m_Position = { -10.0f, 0.0f };//λ��
	glm::vec2 m_Velocity = { 5.0f, 0.0f };//�ٶ�

	float m_EnginePower = 0.5f;//���涯��
	float m_Gravity = 0.4f;//������ʹship���ɵ���

	float m_Time = 0.0f;//ʱ��
	float m_SmokeEmitInterval = 0.4f;//β�����ӷ�����
	float m_SmokeNextEmitTime = m_SmokeEmitInterval;//β�����ӷ�����
	 
	ParticleProps m_SmokeParticle, m_EngineParticle;//��������
	ParticleSystem m_ParticleSystem;//����ϵͳ

	Hazel::Ref<Hazel::Texture2D> m_ShipTexture;//ship����
};