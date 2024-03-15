#include "hzpch.h"
#include "UUID.h"

#include <random>

#include <unordered_map>

namespace Hazel {

    static std::random_device s_RandomDevice;//������豸���������������
    static std::mt19937_64 s_Engine(s_RandomDevice());//��������棬ʹ���������������ʼ��
    static std::uniform_int_distribution<uint64_t> s_UniformDistribution;//���ȷֲ������ɾ��ȷֲ��������

    UUID::UUID()
        : m_UUID(s_UniformDistribution(s_Engine))
    {
    }

    UUID::UUID(uint64_t uuid)
        : m_UUID(uuid)
    {
    }

}