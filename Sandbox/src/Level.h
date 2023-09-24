#pragma once

#include "Player.h"

//�ϰ���
struct Pillar
{
	glm::vec3 TopPosition = { 0.0f, 10.0f, 0.0f };//����λ��
	glm::vec2 TopScale = { 15.0f, 20.0f };//������С

	glm::vec3 BottomPosition = { 10.0f, 10.0f, 0.0f};//�ײ�λ��
	glm::vec2 BottomScale = { 15.0f, 20.0f };//�ײ���С
};

//�ؿ�
class Level
{
public:
	void Init();//�ؿ���ʼ��

	void OnUpdate(Hazel::Timestep ts);//�ؿ�����
	void OnRender();//�ؿ���Ⱦ

	void OnImGuiRender();
	
	bool IsGameOver() const { return m_GameOver; }//�Ƿ����
	void Reset();//����

	Player& GetPlayer() { return m_Player; }//��ȡ���
private:
	void CreatePillar(int index, float offset);//�����ϰ���
	bool CollisionTest();//��ײ���

	void GameOver();//��Ϸ����
private:
	Player m_Player;//���

	bool m_GameOver = false;//�Ƿ����

	float m_PillarTarget = 30.0f;//�ϰ���Ŀ��
	int m_PillarIndex = 0;//�ϰ�������
	glm::vec3 m_PillarHSV = { 0.0f, 0.8f, 0.8f };//�ϰ�����ɫ

	std::vector<Pillar> m_Pillars;//�ϰ���

	Hazel::Ref<Hazel::Texture2D> m_TriangleTexture;//����������
};