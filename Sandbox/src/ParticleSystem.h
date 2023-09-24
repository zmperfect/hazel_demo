#pragma once

#include <Hazel.h>

//��������
struct ParticleProps
{
	glm::vec2 Position;//λ�ã�vec2(0.0f, 0.0f)��ʾ����
	glm::vec2 Velocity, VelocityVariation;//�ٶȣ�vec2(0.0f, 0.0f)��ʾ����
	glm::vec4 ColorBegin, ColorEnd;//������ɫ
	float SizeBegin, SizeEnd, SizeVariation;//���ɴ�С���仯��С
	float LifeTime = 1.0f;//��������
};

//����ϵͳ
class ParticleSystem
{
public:
	ParticleSystem();//��������ϵͳ

	void Emit(const ParticleProps& particleProps);//��������

	void OnUpdate(Hazel::Timestep ts);//����
	void OnRender();//��Ⱦ
private:
	//����
	struct Particle
	{
		glm::vec2 Position;//λ��
		glm::vec2 Velocity;//�ٶ�
		glm::vec4 ColorBegin, ColorEnd;//������ɫ
		float Rotation = 0.0f;//��ת
		float SizeBegin, SizeEnd;//���ɴ�С

		float LifeTime = 1.0f;//��������
		float LifeRemaining = 0.0f;//ʣ����������

		bool Active = false;//�Ƿ񼤻�
	};
	std::vector<Particle> m_ParticlePool;//���ӳ�
	uint32_t m_PoolIndex = 999;//���ӳ�����
};