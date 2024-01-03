#include "hzpch.h"
#include "Math.h"

#define GLM_ENABLE_EXPERIMENTAL //启用实验性功能
#include <glm/gtx/matrix_decompose.hpp>//分解矩阵

namespace Hazel::Math {

    bool Hazel::Math::DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
    {
        //分解变换矩阵 From glm::decompose in matrix_decompose.inl

        using namespace glm;
        using T = float;

        mat4 LocalMatrix(transform);//本地矩阵

        // Normalize the matrix.
        if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))//如果矩阵的第四行第四列为0
            return false;

        // 首先，隔离透视分量
        if (
            epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
            epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
            epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
        {
            //清除透视分量
            LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
            LocalMatrix[3][3] = static_cast<T>(1);
        }

        //接下来是变换分量
        translation = vec3(LocalMatrix[3]);//平移分量
        LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);//清除平移分量

        vec3 Row[3];//行向量

        // 从矩阵中提取缩放分量
        for (length_t i = 0; i < 3; ++i)
            for (length_t j = 0; j < 3; ++j)
            {
                Row[i][j] = LocalMatrix[i][j];
            }

        // 计算缩放向量的长度
        scale.x = length(Row[0]);//x轴缩放
        Row[0] = detail::scale(Row[0], static_cast<T>(1));//单位化x轴缩放向量
        scale.y = length(Row[1]);//y轴缩放
        Row[1] = detail::scale(Row[1], static_cast<T>(1));//单位化y轴缩放向量
        scale.z = length(Row[2]);//z轴缩放
        Row[2] = detail::scale(Row[2], static_cast<T>(1));//单位化z轴缩放向量

        // 检查坐标系是否翻转。 如果行列式为-1，则否定矩阵和缩放因子。
#if 0
        Pdum3 = cross(Row[1], Row[2]);// 交叉乘积
        if (dot(Row[0], Pdum3) < 0)
        {
            for (length_t i = 0; i < 3; i++)
            {
                scale[i] *= static_cast<T>(-1);//缩放因子取反
                Row[i] *= static_cast<T>(-1);//矩阵取反
            }
        }
#endif

        rotation.y = asin(-Row[0][2]);//y轴旋转
        if (cos(rotation.y) != 0)//如果y轴旋转不为0
        {
            rotation.x = atan2(Row[1][2], Row[2][2]);//x轴旋转
            rotation.z = atan2(Row[0][1], Row[0][0]);//z轴旋转
        }
        else
        {
            rotation.x = atan2(-Row[2][0], Row[1][1]);//x轴旋转
            rotation.z = 0;//z轴旋转
        }

        return true;
    }

}

