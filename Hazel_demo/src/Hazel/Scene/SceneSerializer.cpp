#include "hzpch.h"
#include "SceneSerializer.h"

#include "Entity.h"
#include "Components.h"

#include <fstream>

#include <yaml-cpp/yaml.h>

namespace YAML {

    template<>//模板特化
    struct convert<glm::vec2>//glm::vec2的转换
    {
        static Node encode(const glm::vec2& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec2& rhs)
        {
            if (!node.IsSequence() || node.size() != 2)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };


    template<>//模板特化
    struct convert<glm::vec3>//glm::vec3的转换
    {
        static Node encode(const glm::vec3& rhs)//编码
        {
            Node node;//节点
            node.push_back(rhs.x);//x压入节点
            node.push_back(rhs.y);//y压入节点
            node.push_back(rhs.z);//z压入节点
            node.SetStyle(EmitterStyle::Flow);//设置风格为流
            return node;
        }

        static bool decode(const Node& node, glm::vec3& rhs)//解码
        {
            if (!node.IsSequence() || node.size() != 3)//如果不是序列或者长度不为3
                return false;

            rhs.x = node[0].as<float>();//x转换为float
            rhs.y = node[1].as<float>();//y转换为float
            rhs.z = node[2].as<float>();//z转换为float
            return true;
        }
    };

    template<>//模板特化
    struct convert<glm::vec4>//glm::vec4的转换
    {
        static Node encode(const glm::vec4& rhs)//编码
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            node.SetStyle(EmitterStyle::Flow);//设置风格为流
            return node;
        }

        static bool decode(const Node& node, glm::vec4& rhs)//解码
        {
            if (!node.IsSequence() || node.size() != 4)//如果不是序列或者长度不为4
                return false;

            rhs.x = node[0].as<float>();//x转换为float
            rhs.y = node[1].as<float>();//y转换为float
            rhs.z = node[2].as<float>();//z转换为float
            rhs.w = node[3].as<float>();//w转换为float
            return true;
        }
    };

}

namespace Hazel {

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)//重载<<,输出glm::vec2
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)//重载<<,输出glm::vec3
    {
        out << YAML::Flow;//流输出
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;//输出x,y,z
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)//重载<<,输出glm::vec4
    {
        out << YAML::Flow;//流输出
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;//输出x,y,z,w
        return out;
    }

    //将Rigidbody2DComponent::BodyType转换为字符串
    static std::string RigidBody2DBodyTypeToString(Rigidbody2DComponent::BodyType bodyType)
    {
        switch (bodyType)
        {
        case Rigidbody2DComponent::BodyType::Static:    return "Static";
        case Rigidbody2DComponent::BodyType::Dynamic:   return "Dynamic";
        case Rigidbody2DComponent::BodyType::Kinematic: return "Kinematic";
        }

        HZ_CORE_ASSERT(false, "Unknown body type");
        return {};
    }

    //将字符串转换为Rigidbody2DComponent::BodyType
    static Rigidbody2DComponent::BodyType RigidBody2DBodyTypeFromString(const std::string& bodyTypeString)
    {
        if (bodyTypeString == "Static")    return Rigidbody2DComponent::BodyType::Static;
        if (bodyTypeString == "Dynamic")   return Rigidbody2DComponent::BodyType::Dynamic;
        if (bodyTypeString == "Kinematic") return Rigidbody2DComponent::BodyType::Kinematic;

        HZ_CORE_ASSERT(false, "Unknown body type");
        return Rigidbody2DComponent::BodyType::Static;
    }

    SceneSerializer::SceneSerializer(const Ref<Scene>& scene)//构造函数
        : m_Scene(scene)
    {
    }

    static void SerializeEntity(YAML::Emitter& out, Entity entity)//序列化实体
    {
        HZ_CORE_ASSERT(entity.HasComponent<IDComponent>(), "Entity has no ID component,");

        out << YAML::BeginMap;//开始映射

        out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();//实体ID

        if (entity.HasComponent<TagComponent>())//如果有标签组件
        {
            out << YAML::Key << "TagComponent";//标签组件
            out << YAML::BeginMap;//开始映射

            auto& tag = entity.GetComponent<TagComponent>().Tag;//获取标签
            out << YAML::Key << "Tag" << YAML::Value << tag;//输出标签

            out << YAML::EndMap;//结束映射
        }

        if (entity.HasComponent<TransformComponent>())//如果有变换组件
        {
            out << YAML::Key << "TransformComponent";//变换组件
            out << YAML::BeginMap;//开始映射

            auto& tc = entity.GetComponent<TransformComponent>();//变换组件
            out << YAML::Key << "Translation" << YAML::Value << tc.Translation;//平移
            out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;//旋转
            out << YAML::Key << "Scale" << YAML::Value << tc.Scale;//缩放

            out << YAML::EndMap;//结束映射
        }

        if (entity.HasComponent<CameraComponent>())//如果有相机组件
        {
            out << YAML::Key << "CameraComponent";//相机组件
            out << YAML::BeginMap;//开始映射

            auto& cameraComponent = entity.GetComponent<CameraComponent>();//获取相机组件
            auto& camera = cameraComponent.Camera;//相机

            out << YAML::Key << "Camera" << YAML::Value;//相机
            out << YAML::BeginMap;//开始映射
            out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();//投影类型
            out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();//透视垂直视角
            out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();//透视近裁剪面
            out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();//透视远裁剪面
            out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();//正交大小
            out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();//正交近裁剪面
            out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();//正交远裁剪面
            out << YAML::EndMap;//结束映射

            out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;//主相机
            out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;//固定纵横比

            out << YAML::EndMap;//结束映射
        }

        if (entity.HasComponent<SpriteRendererComponent>())//如果有精灵渲染器组件
        {
            out << YAML::Key << "SpriteRendererComponent";
            out << YAML::BeginMap;//开始映射

            auto& src = entity.GetComponent<SpriteRendererComponent>();//精灵渲染器组件
            out << YAML::Key << "Color" << YAML::Value << src.Color;//颜色

            out << YAML::EndMap;//结束映射
        }

        if (entity.HasComponent<CircleRendererComponent>())//如果有圆形渲染器组件
        {
            out << YAML::Key << "CircleRendererComponent";
            out << YAML::BeginMap; // CircleRendererComponent

            auto& circleRendererComponent = entity.GetComponent<CircleRendererComponent>();
            out << YAML::Key << "Color" << YAML::Value << circleRendererComponent.Color;
            out << YAML::Key << "Thickness" << YAML::Value << circleRendererComponent.Thickness;
            out << YAML::Key << "Fade" << YAML::Value << circleRendererComponent.Fade;

            out << YAML::EndMap; // CircleRendererComponent
        }

        if (entity.HasComponent<Rigidbody2DComponent>())//如果有刚体组件
        {
            out << YAML::Key << "Rigidbody2DComponent";
            out << YAML::BeginMap; // Rigidbody2DComponent

            auto& rb2dComponent = entity.GetComponent<Rigidbody2DComponent>();
            out << YAML::Key << "BodyType" << YAML::Value << RigidBody2DBodyTypeToString(rb2dComponent.Type);
            out << YAML::Key << "FixedRotation" << YAML::Value << rb2dComponent.FixedRotation;

            out << YAML::EndMap; // Rigidbody2DComponent
        }

        if (entity.HasComponent<BoxCollider2DComponent>())//如果有刚体碰撞组件
        {
            out << YAML::Key << "BoxCollider2DComponent";
            out << YAML::BeginMap; // BoxCollider2DComponent

            auto& bc2dComponent = entity.GetComponent<BoxCollider2DComponent>();
            out << YAML::Key << "Offset" << YAML::Value << bc2dComponent.Offset;
            out << YAML::Key << "Size" << YAML::Value << bc2dComponent.Size;
            out << YAML::Key << "Density" << YAML::Value << bc2dComponent.Density;
            out << YAML::Key << "Friction" << YAML::Value << bc2dComponent.Friction;
            out << YAML::Key << "Restitution" << YAML::Value << bc2dComponent.Restitution;
            out << YAML::Key << "RestitutionThreshold" << YAML::Value << bc2dComponent.RestitutionThreshold;

            out << YAML::EndMap; // BoxCollider2DComponent
        }

        if (entity.HasComponent<CircleCollider2DComponent>())
        {
            out << YAML::Key << "CircleCollider2DComponent";
            out << YAML::BeginMap; // CircleCollider2DComponent

            auto& cc2dComponent = entity.GetComponent<CircleCollider2DComponent>();
            out << YAML::Key << "Offset" << YAML::Value << cc2dComponent.Offset;
            out << YAML::Key << "Radius" << YAML::Value << cc2dComponent.Radius;
            out << YAML::Key << "Density" << YAML::Value << cc2dComponent.Density;
            out << YAML::Key << "Friction" << YAML::Value << cc2dComponent.Friction;
            out << YAML::Key << "Restitution" << YAML::Value << cc2dComponent.Restitution;
            out << YAML::Key << "RestitutionThreshold" << YAML::Value << cc2dComponent.RestitutionThreshold;

            out << YAML::EndMap; // CircleCollider2DComponent
        }

        out << YAML::EndMap;//结束映射
    }

    void SceneSerializer::Serialize(const std::string& filepath)
    {
        YAML::Emitter out;//输出
        out << YAML::BeginMap;//开始映射

        out << YAML::Key << "Scene" << YAML::Value << "Untitled";//场景
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;//实体

        m_Scene->m_Registry.each([&](auto entityID)//遍历实体
        {
            Entity entity = { entityID, m_Scene.get() };//实体
            if (!entity)//如果实体无效
                return;

            SerializeEntity(out, entity);//序列化实体
        });

        out << YAML::EndSeq;//结束序列
        out << YAML::EndMap;//结束映射

        std::ofstream fout(filepath);//输出文件流
        fout << out.c_str();//输出
    }

    void SceneSerializer::SerializeRuntime(const std::string& filepath)
    {
        //无法序列化运行时场景
        HZ_CORE_ASSERT(false);//断言失败
    }

    // 反序列化场景
    bool SceneSerializer::Deserialize(const std::string& filepath)
    {
        YAML::Node data;// 数据
        try
        {
            data = YAML::LoadFile(filepath);//加载文件
        }
        catch (YAML::ParserException e)//解析异常
        {
            HZ_CORE_ERROR("YAML::ParserException: {0}", e.what());//错误
            return false;//返回失败
        }
        if (!data["Scene"])//如果没有场景
            return false;

        std::string sceneName = data["Scene"].as<std::string>();//场景名
        HZ_CORE_TRACE("Deserializing scene '{0}'", sceneName);//跟踪

        auto entities = data["Entities"];//实体
        if (entities)
        {
            for (auto entity : entities)
            {
                uint64_t uuid = entity["Entity"].as<uint64_t>();//实体ID

                std::string name;//名称
                auto tagComponent = entity["TagComponent"];//标签组件
                if (tagComponent)
                    name = tagComponent["Tag"].as<std::string>();//名称

                HZ_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);//跟踪

                Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);//创建实体

                auto transformComponent = entity["TransformComponent"];//变换组件
                if (transformComponent)
                {
                    // Entities always have transforms
                    auto& tc = deserializedEntity.GetComponent<TransformComponent>();//变换组件
                    tc.Translation = transformComponent["Translation"].as<glm::vec3>();//平移
                    tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();//旋转
                    tc.Scale = transformComponent["Scale"].as<glm::vec3>();//缩放
                }

                auto cameraComponent = entity["CameraComponent"];//相机组件
                if (cameraComponent)
                {
                    auto& cc = deserializedEntity.AddComponent<CameraComponent>();//相机组件

                    auto& cameraProps = cameraComponent["Camera"];//相机属性
                    cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());//投影类型

                    cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());//透视垂直视角
                    cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());//透视近裁剪面
                    cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());//透视远裁剪面

                    cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());//正交大小
                    cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());//正交近裁剪面
                    cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());//正交远裁剪面

                    cc.Primary = cameraComponent["Primary"].as<bool>();//主相机
                    cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();//固定纵横比
                }

                auto spriteRendererComponent = entity["SpriteRendererComponent"];//精灵渲染器组件
                if (spriteRendererComponent)
                {
                    auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();//精灵渲染器组件
                    src.Color = spriteRendererComponent["Color"].as<glm::vec4>();//颜色
                }

                auto circleRendererComponent = entity["CircleRendererComponent"];//圆形渲染器组件
                if (circleRendererComponent)
                {
                    auto& crc = deserializedEntity.AddComponent<CircleRendererComponent>();
                    crc.Color = circleRendererComponent["Color"].as<glm::vec4>();
                    crc.Thickness = circleRendererComponent["Thickness"].as<float>();
                    crc.Fade = circleRendererComponent["Fade"].as<float>();
                }

                auto rigidbody2DComponent = entity["Rigidbody2DComponent"];//刚体组件
                if (rigidbody2DComponent)
                {
                    auto& rb2d = deserializedEntity.AddComponent<Rigidbody2DComponent>();
                    rb2d.Type = RigidBody2DBodyTypeFromString(rigidbody2DComponent["BodyType"].as<std::string>());
                    rb2d.FixedRotation = rigidbody2DComponent["FixedRotation"].as<bool>();
                }

                auto boxCollider2DComponent = entity["BoxCollider2DComponent"];//刚体碰撞组件
                if (boxCollider2DComponent)
                {
                    auto& bc2d = deserializedEntity.AddComponent<BoxCollider2DComponent>();
                    bc2d.Offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
                    bc2d.Size = boxCollider2DComponent["Size"].as<glm::vec2>();
                    bc2d.Density = boxCollider2DComponent["Density"].as<float>();
                    bc2d.Friction = boxCollider2DComponent["Friction"].as<float>();
                    bc2d.Restitution = boxCollider2DComponent["Restitution"].as<float>();
                    bc2d.RestitutionThreshold = boxCollider2DComponent["RestitutionThreshold"].as<float>();
                }

                auto circleCollider2DComponent = entity["CircleCollider2DComponent"];
                if (circleCollider2DComponent)
                {
                    auto& cc2d = deserializedEntity.AddComponent<CircleCollider2DComponent>();
                    cc2d.Offset = circleCollider2DComponent["Offset"].as<glm::vec2>();
                    cc2d.Radius = circleCollider2DComponent["Radius"].as<float>();
                    cc2d.Density = circleCollider2DComponent["Density"].as<float>();
                    cc2d.Friction = circleCollider2DComponent["Friction"].as<float>();
                    cc2d.Restitution = circleCollider2DComponent["Restitution"].as<float>();
                    cc2d.RestitutionThreshold = circleCollider2DComponent["RestitutionThreshold"].as<float>();
                }
            }
        }

        return true;
    }

    bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
    {
        // No implemented
        HZ_CORE_ASSERT(false);//断言失败
        return false;//返回失败
    }
    
}