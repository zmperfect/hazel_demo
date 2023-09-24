#pragma once

#include "Player.h"

//障碍物
struct Pillar
{
	glm::vec3 TopPosition = { 0.0f, 10.0f, 0.0f };//顶部位置
	glm::vec2 TopScale = { 15.0f, 20.0f };//顶部大小

	glm::vec3 BottomPosition = { 10.0f, 10.0f, 0.0f};//底部位置
	glm::vec2 BottomScale = { 15.0f, 20.0f };//底部大小
};

//关卡
class Level
{
public:
	void Init();//关卡初始化

	void OnUpdate(Hazel::Timestep ts);//关卡更新
	void OnRender();//关卡渲染

	void OnImGuiRender();
	
	bool IsGameOver() const { return m_GameOver; }//是否结束
	void Reset();//重置

	Player& GetPlayer() { return m_Player; }//获取玩家
private:
	void CreatePillar(int index, float offset);//创建障碍物
	bool CollisionTest();//碰撞检测

	void GameOver();//游戏结束
private:
	Player m_Player;//玩家

	bool m_GameOver = false;//是否结束

	float m_PillarTarget = 30.0f;//障碍物目标
	int m_PillarIndex = 0;//障碍物索引
	glm::vec3 m_PillarHSV = { 0.0f, 0.8f, 0.8f };//障碍物颜色

	std::vector<Pillar> m_Pillars;//障碍物

	Hazel::Ref<Hazel::Texture2D> m_TriangleTexture;//三角形纹理
};