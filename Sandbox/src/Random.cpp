#include "Random.h"

std::mt19937 Random::s_RandomEngine;//随机数发生器
std::uniform_int_distribution<std::mt19937::result_type> Random::s_Distribution;//随机数分布
