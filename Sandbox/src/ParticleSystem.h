#pragma once

#include <Hazel.h>

//粒子属性
struct ParticleProps
{
	glm::vec2 Position;//位置，vec2(0.0f, 0.0f)表示中心
	glm::vec2 Velocity, VelocityVariation;//速度，vec2(0.0f, 0.0f)表示中心
	glm::vec4 ColorBegin, ColorEnd;//过渡颜色
	float SizeBegin, SizeEnd, SizeVariation;//过渡大小，变化大小
	float LifeTime = 1.0f;//生命周期
};

//粒子系统
class ParticleSystem
{
public:
	ParticleSystem();//构造粒子系统

	void Emit(const ParticleProps& particleProps);//发射粒子

	void OnUpdate(Hazel::Timestep ts);//更新
	void OnRender();//渲染
private:
	//粒子
	struct Particle
	{
		glm::vec2 Position;//位置
		glm::vec2 Velocity;//速度
		glm::vec4 ColorBegin, ColorEnd;//过渡颜色
		float Rotation = 0.0f;//旋转
		float SizeBegin, SizeEnd;//过渡大小

		float LifeTime = 1.0f;//生命周期
		float LifeRemaining = 0.0f;//剩余生命周期

		bool Active = false;//是否激活
	};
	std::vector<Particle> m_ParticlePool;//粒子池
	uint32_t m_PoolIndex = 999;//粒子池索引
};