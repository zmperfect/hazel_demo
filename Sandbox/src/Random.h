#pragma once

#include <random>

//随机数发生器
class Random
{
public:
	static void Init()
	{
		s_RandomEngine.seed(std::random_device()());//随机数发生器种子
	}

	static float Float()
	{
		return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();//随机数发生器，返回[0.0f, 1.0f]之间的随机数
	}
private:
	static std::mt19937 s_RandomEngine;//随机数发生器
	static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;//随机数分布
};