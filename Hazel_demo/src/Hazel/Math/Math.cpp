#include "hzpch.h"
#include "Math.h"

#define GLM_ENABLE_EXPERIMENTAL //����ʵ���Թ���
#include <glm/gtx/matrix_decompose.hpp>//�ֽ����

namespace Hazel::Math {

    bool Hazel::Math::DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
    {
        //�ֽ�任���� From glm::decompose in matrix_decompose.inl

        using namespace glm;
        using T = float;

        mat4 LocalMatrix(transform);//���ؾ���

        // Normalize the matrix.
        if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))//�������ĵ����е�����Ϊ0
            return false;

        // ���ȣ�����͸�ӷ���
        if (
            epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
            epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
            epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
        {
            //���͸�ӷ���
            LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
            LocalMatrix[3][3] = static_cast<T>(1);
        }

        //�������Ǳ任����
        translation = vec3(LocalMatrix[3]);//ƽ�Ʒ���
        LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);//���ƽ�Ʒ���

        vec3 Row[3];//������

        // �Ӿ�������ȡ���ŷ���
        for (length_t i = 0; i < 3; ++i)
            for (length_t j = 0; j < 3; ++j)
            {
                Row[i][j] = LocalMatrix[i][j];
            }

        // �������������ĳ���
        scale.x = length(Row[0]);//x������
        Row[0] = detail::scale(Row[0], static_cast<T>(1));//��λ��x����������
        scale.y = length(Row[1]);//y������
        Row[1] = detail::scale(Row[1], static_cast<T>(1));//��λ��y����������
        scale.z = length(Row[2]);//z������
        Row[2] = detail::scale(Row[2], static_cast<T>(1));//��λ��z����������

        // �������ϵ�Ƿ�ת�� �������ʽΪ-1����񶨾�����������ӡ�
#if 0
        Pdum3 = cross(Row[1], Row[2]);// ����˻�
        if (dot(Row[0], Pdum3) < 0)
        {
            for (length_t i = 0; i < 3; i++)
            {
                scale[i] *= static_cast<T>(-1);//��������ȡ��
                Row[i] *= static_cast<T>(-1);//����ȡ��
            }
        }
#endif

        rotation.y = asin(-Row[0][2]);//y����ת
        if (cos(rotation.y) != 0)//���y����ת��Ϊ0
        {
            rotation.x = atan2(Row[1][2], Row[2][2]);//x����ת
            rotation.z = atan2(Row[0][1], Row[0][0]);//z����ת
        }
        else
        {
            rotation.x = atan2(-Row[2][0], Row[1][1]);//x����ת
            rotation.z = 0;//z����ת
        }

        return true;
    }

}

