#pragma once

#include "Hazel/Scene/Scene.h"

namespace Hazel {

    //�������л���
    class SceneSerializer
    {
    public:
        SceneSerializer(const Ref<Scene>& scene);//���캯��

        void Serialize(const std::string& filepath);//���л�
        void SerializeRuntime(const std::string& filepath);//����ʱ���л�

        bool Deserialize(const std::string& filepath);//�����л�
        bool DeserializeRuntime(const std::string& filepath);//����ʱ�����л�
    private:
        Ref<Scene> m_Scene;//����
    };

}