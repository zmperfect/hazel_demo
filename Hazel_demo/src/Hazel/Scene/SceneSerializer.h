#pragma once

#include "Hazel/Scene/Scene.h"

namespace Hazel {

    //场景序列化类
    class SceneSerializer
    {
    public:
        SceneSerializer(const Ref<Scene>& scene);//构造函数

        void Serialize(const std::string& filepath);//序列化
        void SerializeRuntime(const std::string& filepath);//运行时序列化

        bool Deserialize(const std::string& filepath);//反序列化
        bool DeserializeRuntime(const std::string& filepath);//运行时反序列化
    private:
        Ref<Scene> m_Scene;//场景
    };

}