#include "hzpch.h"
#include "UUID.h"

#include <random>

#include <unordered_map>

namespace Hazel {

    static std::random_device s_RandomDevice;//随机数设备，生成随机数种子
    static std::mt19937_64 s_Engine(s_RandomDevice());//随机数引擎，使用随机数种子来初始化
    static std::uniform_int_distribution<uint64_t> s_UniformDistribution;//均匀分布，生成均匀分布的随机数

    UUID::UUID()
        : m_UUID(s_UniformDistribution(s_Engine))
    {
    }

    UUID::UUID(uint64_t uuid)
        : m_UUID(uuid)
    {
    }

}