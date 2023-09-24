#include "Level.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace Hazel;

//将HSV颜色转换为RGB颜色
static glm::vec4 HSVtoRGB(const glm::vec3& hsv) {
	int H = (int)(hsv.x * 360.0f);//获取Hue（色调、色相）
	double S = hsv.y;//获取Saturation（饱和度、色彩纯净度）
	double V = hsv.z;//获取Value（明度）

	double C = S * V;
	double X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
	double m = V - C;
	double Rs, Gs, Bs;//定义RGB

	if (H >= 0 && H < 60) {
		Rs = C;
		Gs = X;
		Bs = 0;
	}
	else if (H >= 60 && H < 120) {
		Rs = X;
		Gs = C;
		Bs = 0;
	}
	else if (H >= 120 && H < 180) {
		Rs = 0;
		Gs = C;
		Bs = X;
	}
	else if (H >= 180 && H < 240) {
		Rs = 0;
		Gs = X;
		Bs = C;
	}
	else if (H >= 240 && H < 300) {
		Rs = X;
		Gs = 0;
		Bs = C;
	}
	else {
		Rs = C;
		Gs = 0;
		Bs = X;
	}

	return { (Rs + m), (Gs + m), (Bs + m), 1.0f };//返回RGBA
}

//判断点是否在三角形内
static bool PointInTri(const glm::vec2& p, glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2)
{
	float s = p0.y * p2.x - p0.x * p2.y + (p2.y - p0.y) * p.x + (p0.x - p2.x) * p.y;
	float t = p0.x * p1.y - p0.y * p1.x + (p0.y - p1.y) * p.x + (p1.x - p0.x) * p.y;

	if ((s < 0) != (t < 0))
		return false;

	float A = -p1.y * p2.x + p0.y * (p2.x - p1.x) + p0.x * (p1.y - p2.y) + p1.x * p2.y;

	return A < 0 ?
		(s <= 0 && s + t >= A) :
		(s >= 0 && s + t <= A);
}

//关卡初始化
void Level::Init()
{
	m_TriangleTexture = Texture2D::Create("assets/textures/Triangle.png");//加载三角形纹理
	m_Player.LoadAssets();//加载玩家资源

	m_Pillars.resize(5);//设置障碍物数量
	for (int i = 0; i < 5; i++)
		CreatePillar(i, i * 10.0f);//创建障碍物
}

//更新关卡
void Level::OnUpdate(Hazel::Timestep ts)
{
	m_Player.OnUpdate(ts);//更新玩家

	//检测碰撞
	if (CollisionTest())
	{
		GameOver();
		return;
	}

	//隔一段更新障碍物的颜色
	m_PillarHSV.x += 0.1f * ts;
	if (m_PillarHSV.x > 1.0f)
		m_PillarHSV.x = 0.0f;

	//根据玩家位置更新障碍物
	if (m_Player.GetPosition().x > m_PillarTarget)
	{
		CreatePillar(m_PillarIndex, m_PillarTarget + 20.0f);
		m_PillarIndex = ++m_PillarIndex % m_Pillars.size();//循环更新障碍物
		m_PillarTarget += 10.0f;//更新障碍物的位置
	}
}

//渲染关卡
void Level::OnRender()
{
	const auto& playerPos = m_Player.GetPosition();//获取玩家位置

	glm::vec4 color = HSVtoRGB(m_PillarHSV);//障碍物颜色转换成RGBA

	// 背景
	Renderer2D::DrawQuad({ playerPos.x, 0.0f, -0.8f }, { 50.0f, 50.0f }, { 0.3f, 0.3f, 0.3f, 1.0f });//以玩家位置为中心绘制背景

	// Floor and ceiling
	Renderer2D::DrawQuad({ playerPos.x,  34.0f }, { 50.0f, 50.0f }, color);//绘制天花板
	Renderer2D::DrawQuad({ playerPos.x, -34.0f }, { 50.0f, 50.0f }, color);//绘制地板

	// Pillars
	for (auto& pillar : m_Pillars)
	{
		Renderer2D::DrawQuad(pillar.TopPosition, pillar.TopScale, glm::radians(180.0f), m_TriangleTexture, color);//绘制障碍物，位置为pillar.TopPosition，大小为pillar.TopScale，旋转180度，使用三角形纹理，颜色为color
		Renderer2D::DrawQuad(pillar.BottomPosition, pillar.BottomScale, 0.0f, m_TriangleTexture, color);//绘制障碍物，位置为pillar.BottomPosition，大小为pillar.BottomScale，旋转0度，使用三角形纹理，颜色为color
	}

	m_Player.OnRender();//玩家渲染
}

//ImGui渲染
void Level::OnImGuiRender()
{
	m_Player.OnImGuiRender();
}

//创建障碍物
void Level::CreatePillar(int index, float offset)
{
	Pillar& pillar = m_Pillars[index];//获取障碍物
	pillar.TopPosition.x = offset;//设置顶部障碍物位置
	pillar.BottomPosition.x = offset;//设置底部障碍物位置
	pillar.TopPosition.z = index * 0.1f - 0.5f;
	pillar.BottomPosition.z = index * 0.1f - 0.5f + 0.05f;

	float center = Random::Float() * 35.0f - 17.5f;//随机生成障碍物中心位置
	float gap = 2.0f + Random::Float() * 5.0f;//随机生成障碍物间隙

	pillar.TopPosition.y = 10.0f - ((10.0f - center) * 0.2f) + gap * 0.5f;//设置顶部障碍物位置
	pillar.BottomPosition.y = -10.0f - ((-10.0f - center) * 0.2f) - gap * 0.5f;//设置底部障碍物位置
}

//碰撞检测
bool Level::CollisionTest()
{
	if (glm::abs(m_Player.GetPosition().y) > 8.5f)//玩家是否超出边界
		return true;

	glm::vec4 playerVertices[4] = {		//玩家顶点
		{ -0.5f, -0.5f, 0.0f, 1.0f },
		{  0.5f, -0.5f, 0.0f, 1.0f },
		{  0.5f,  0.5f, 0.0f, 1.0f },
		{ -0.5f,  0.5f, 0.0f, 1.0f }
	};

	const auto& pos = m_Player.GetPosition();//获取玩家位置
	glm::vec4 playerTransformedVerts[4];//玩家变换后的顶点
	//遍历玩家顶点，进行变换
	for (int i = 0; i < 4; i++)
	{
		//玩家变换后的顶点等于玩家位置变换矩阵*玩家顶点选装矩阵*玩家顶点缩放矩阵*玩家顶点
		playerTransformedVerts[i] = glm::translate(glm::mat4(1.0f), { pos.x, pos.y, 0.0f })
			* glm::rotate(glm::mat4(1.0f), glm::radians(m_Player.GetRotation()), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { 1.0f, 1.3f, 1.0f })
			* playerVertices[i];
	}


	// To match Triangle.png (each corner is 10% from the texture edge)
	glm::vec4 pillarVertices[3] = {	//pillar顶点，三角形
		{ -0.5f + 0.1f, -0.5f + 0.1f, 0.0f, 1.0f },
		{  0.5f - 0.1f, -0.5f + 0.1f, 0.0f, 1.0f },
		{  0.0f + 0.0f,  0.5f - 0.1f, 0.0f, 1.0f },
	};

	//遍历障碍物
	for (auto& p : m_Pillars)
	{
		glm::vec2 tri[3];//三角形顶点
		
		// Top pillars
		for (int i = 0; i < 3; i++)
		{
			//同玩家变换一样
			tri[i] = glm::translate(glm::mat4(1.0f), { p.TopPosition.x, p.TopPosition.y, 0.0f })
				* glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), { 0.0f, 0.0f, 1.0f })
				* glm::scale(glm::mat4(1.0f), { p.TopScale.x, p.TopScale.y, 1.0f })
				* pillarVertices[i];
		}

		//遍历玩家变换后的顶点
		for (auto& vert : playerTransformedVerts)
		{
			//判断玩家顶点是否在障碍物三角形内
			if (PointInTri({ vert.x, vert.y }, tri[0], tri[1], tri[2]))
				return true;
		}

		// Bottom pillars
		for (int i = 0; i < 3; i++)
		{
			tri[i] = glm::translate(glm::mat4(1.0f), { p.BottomPosition.x, p.BottomPosition.y, 0.0f })
				* glm::scale(glm::mat4(1.0f), { p.BottomScale.x, p.BottomScale.y, 1.0f })
				* pillarVertices[i];
		}

		//遍历玩家变换后的顶点
		for (auto& vert : playerTransformedVerts)
		{
			//判断玩家顶点是否在障碍物三角形内
			if (PointInTri({ vert.x, vert.y }, tri[0], tri[1], tri[2]))
				return true;
		}

	}
	return false;
}

//游戏结束
void Level::GameOver()
{
	m_GameOver = true;
}

//重置
void Level::Reset()
{
	m_GameOver = false;

	m_Player.Reset();

	m_PillarTarget = 30.0f;
	m_PillarIndex = 0;
	for (int i = 0; i < 5; i++)
		CreatePillar(i, i * 10.0f);
}