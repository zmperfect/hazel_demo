#include "ParticleSystem.h"

#include "Random.h"

#define GLM_ENABLE_EXPERIMENTAL	//启用实验性功能
#include <glm/gtx/compatibility.hpp>	//兼容性库，包含lerp函数

ParticleSystem::ParticleSystem()
{
	m_ParticlePool.resize(1000);//粒子池大小
}

void ParticleSystem::Emit(const ParticleProps& particleProps)//发射粒子
{
	Particle& particle = m_ParticlePool[m_PoolIndex];//获取粒子池中的粒子
	particle.Active = true;//激活粒子
	particle.Position = particleProps.Position;//位置
	particle.Rotation = Random::Float() * 2.0f * glm::pi<float>();//旋转，随机角度

	// Velocity
	particle.Velocity = particleProps.Velocity;
	particle.Velocity.x += particleProps.VelocityVariation.x * (Random::Float() - 0.5f);//粒子速度x轴方向上的变化
	particle.Velocity.y += particleProps.VelocityVariation.y * (Random::Float() - 0.5f);//粒子速度y轴方向上的变化

	// Color
	particle.ColorBegin = particleProps.ColorBegin;//粒子开始颜色
	particle.ColorEnd = particleProps.ColorEnd;//粒子结束颜色

	// Size
	particle.SizeBegin = particleProps.SizeBegin + particleProps.SizeVariation * (Random::Float() - 0.5f);//粒子开始大小
	particle.SizeEnd = particleProps.SizeEnd;//粒子结束大小

	// Life
	particle.LifeTime = particleProps.LifeTime;//粒子生命周期
	particle.LifeRemaining = particleProps.LifeTime;//粒子剩余生命周期

	m_PoolIndex = --m_PoolIndex % m_ParticlePool.size();//粒子池索引,循环使用
}

void ParticleSystem::OnUpdate(Hazel::Timestep ts)//更新
{
	for (auto& particle : m_ParticlePool)//遍历粒子池
	{
		if (!particle.Active)
			continue;

		if (particle.LifeRemaining <= 0.0f)//粒子生命周期结束
		{
			particle.Active = false;
			continue;
		}

		particle.LifeRemaining -= ts;//粒子剩余生命周期减少
		particle.Position += particle.Velocity * (float)ts;//粒子位置变化
		particle.Rotation += 0.01f * ts;//粒子旋转变化
	}
}

void ParticleSystem::OnRender()//渲染
{
	for (auto& particle : m_ParticlePool)//遍历粒子池
	{
		if (!particle.Active)
			continue;

		float life = particle.LifeRemaining / particle.LifeTime;//剩余生命周期除以生命周期
		glm::vec4 color = glm::lerp(particle.ColorEnd, particle.ColorBegin, life);//过渡颜色
		color.a = color.a * life;//透明度，随生命周期变化

		float size = glm::lerp(particle.SizeEnd, particle.SizeBegin, life);//大小
		Hazel::Renderer2D::DrawQuad(particle.Position, { size, size }, particle.Rotation, color);//画粒子，(粒子位置,{x方向大小,y方向大小},旋转角度,颜色)
	}
}
