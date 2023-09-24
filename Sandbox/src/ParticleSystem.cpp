#include "ParticleSystem.h"

#include "Random.h"

#define GLM_ENABLE_EXPERIMENTAL	//����ʵ���Թ���
#include <glm/gtx/compatibility.hpp>	//�����Կ⣬����lerp����

ParticleSystem::ParticleSystem()
{
	m_ParticlePool.resize(1000);//���ӳش�С
}

void ParticleSystem::Emit(const ParticleProps& particleProps)//��������
{
	Particle& particle = m_ParticlePool[m_PoolIndex];//��ȡ���ӳ��е�����
	particle.Active = true;//��������
	particle.Position = particleProps.Position;//λ��
	particle.Rotation = Random::Float() * 2.0f * glm::pi<float>();//��ת������Ƕ�

	// Velocity
	particle.Velocity = particleProps.Velocity;
	particle.Velocity.x += particleProps.VelocityVariation.x * (Random::Float() - 0.5f);//�����ٶ�x�᷽���ϵı仯
	particle.Velocity.y += particleProps.VelocityVariation.y * (Random::Float() - 0.5f);//�����ٶ�y�᷽���ϵı仯

	// Color
	particle.ColorBegin = particleProps.ColorBegin;//���ӿ�ʼ��ɫ
	particle.ColorEnd = particleProps.ColorEnd;//���ӽ�����ɫ

	// Size
	particle.SizeBegin = particleProps.SizeBegin + particleProps.SizeVariation * (Random::Float() - 0.5f);//���ӿ�ʼ��С
	particle.SizeEnd = particleProps.SizeEnd;//���ӽ�����С

	// Life
	particle.LifeTime = particleProps.LifeTime;//������������
	particle.LifeRemaining = particleProps.LifeTime;//����ʣ����������

	m_PoolIndex = --m_PoolIndex % m_ParticlePool.size();//���ӳ�����,ѭ��ʹ��
}

void ParticleSystem::OnUpdate(Hazel::Timestep ts)//����
{
	for (auto& particle : m_ParticlePool)//�������ӳ�
	{
		if (!particle.Active)
			continue;

		if (particle.LifeRemaining <= 0.0f)//�����������ڽ���
		{
			particle.Active = false;
			continue;
		}

		particle.LifeRemaining -= ts;//����ʣ���������ڼ���
		particle.Position += particle.Velocity * (float)ts;//����λ�ñ仯
		particle.Rotation += 0.01f * ts;//������ת�仯
	}
}

void ParticleSystem::OnRender()//��Ⱦ
{
	for (auto& particle : m_ParticlePool)//�������ӳ�
	{
		if (!particle.Active)
			continue;

		float life = particle.LifeRemaining / particle.LifeTime;//ʣ���������ڳ�����������
		glm::vec4 color = glm::lerp(particle.ColorEnd, particle.ColorBegin, life);//������ɫ
		color.a = color.a * life;//͸���ȣ����������ڱ仯

		float size = glm::lerp(particle.SizeEnd, particle.SizeBegin, life);//��С
		Hazel::Renderer2D::DrawQuad(particle.Position, { size, size }, particle.Rotation, color);//�����ӣ�(����λ��,{x�����С,y�����С},��ת�Ƕ�,��ɫ)
	}
}
