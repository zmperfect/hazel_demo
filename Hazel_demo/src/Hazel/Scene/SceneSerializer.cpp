#include "hzpch.h"
#include "SceneSerializer.h"

#include "Entity.h"
#include "Components.h"

#include <fstream>

#include <yaml-cpp/yaml.h>

namespace YAML {

    template<>//ģ���ػ�
    struct convert<glm::vec2>//glm::vec2��ת��
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


    template<>//ģ���ػ�
    struct convert<glm::vec3>//glm::vec3��ת��
    {
        static Node encode(const glm::vec3& rhs)//����
        {
            Node node;//�ڵ�
            node.push_back(rhs.x);//xѹ��ڵ�
            node.push_back(rhs.y);//yѹ��ڵ�
            node.push_back(rhs.z);//zѹ��ڵ�
            node.SetStyle(EmitterStyle::Flow);//���÷��Ϊ��
            return node;
        }

        static bool decode(const Node& node, glm::vec3& rhs)//����
        {
            if (!node.IsSequence() || node.size() != 3)//����������л��߳��Ȳ�Ϊ3
                return false;

            rhs.x = node[0].as<float>();//xת��Ϊfloat
            rhs.y = node[1].as<float>();//yת��Ϊfloat
            rhs.z = node[2].as<float>();//zת��Ϊfloat
            return true;
        }
    };

    template<>//ģ���ػ�
    struct convert<glm::vec4>//glm::vec4��ת��
    {
        static Node encode(const glm::vec4& rhs)//����
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            node.SetStyle(EmitterStyle::Flow);//���÷��Ϊ��
            return node;
        }

        static bool decode(const Node& node, glm::vec4& rhs)//����
        {
            if (!node.IsSequence() || node.size() != 4)//����������л��߳��Ȳ�Ϊ4
                return false;

            rhs.x = node[0].as<float>();//xת��Ϊfloat
            rhs.y = node[1].as<float>();//yת��Ϊfloat
            rhs.z = node[2].as<float>();//zת��Ϊfloat
            rhs.w = node[3].as<float>();//wת��Ϊfloat
            return true;
        }
    };

}

namespace Hazel {

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)//����<<,���glm::vec2
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)//����<<,���glm::vec3
    {
        out << YAML::Flow;//�����
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;//���x,y,z
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)//����<<,���glm::vec4
    {
        out << YAML::Flow;//�����
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;//���x,y,z,w
        return out;
    }

    //��Rigidbody2DComponent::BodyTypeת��Ϊ�ַ���
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

    //���ַ���ת��ΪRigidbody2DComponent::BodyType
    static Rigidbody2DComponent::BodyType RigidBody2DBodyTypeFromString(const std::string& bodyTypeString)
    {
        if (bodyTypeString == "Static")    return Rigidbody2DComponent::BodyType::Static;
        if (bodyTypeString == "Dynamic")   return Rigidbody2DComponent::BodyType::Dynamic;
        if (bodyTypeString == "Kinematic") return Rigidbody2DComponent::BodyType::Kinematic;

        HZ_CORE_ASSERT(false, "Unknown body type");
        return Rigidbody2DComponent::BodyType::Static;
    }

    SceneSerializer::SceneSerializer(const Ref<Scene>& scene)//���캯��
        : m_Scene(scene)
    {
    }

    static void SerializeEntity(YAML::Emitter& out, Entity entity)//���л�ʵ��
    {
        HZ_CORE_ASSERT(entity.HasComponent<IDComponent>(), "Entity has no ID component,");

        out << YAML::BeginMap;//��ʼӳ��

        out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();//ʵ��ID

        if (entity.HasComponent<TagComponent>())//����б�ǩ���
        {
            out << YAML::Key << "TagComponent";//��ǩ���
            out << YAML::BeginMap;//��ʼӳ��

            auto& tag = entity.GetComponent<TagComponent>().Tag;//��ȡ��ǩ
            out << YAML::Key << "Tag" << YAML::Value << tag;//�����ǩ

            out << YAML::EndMap;//����ӳ��
        }

        if (entity.HasComponent<TransformComponent>())//����б任���
        {
            out << YAML::Key << "TransformComponent";//�任���
            out << YAML::BeginMap;//��ʼӳ��

            auto& tc = entity.GetComponent<TransformComponent>();//�任���
            out << YAML::Key << "Translation" << YAML::Value << tc.Translation;//ƽ��
            out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;//��ת
            out << YAML::Key << "Scale" << YAML::Value << tc.Scale;//����

            out << YAML::EndMap;//����ӳ��
        }

        if (entity.HasComponent<CameraComponent>())//�����������
        {
            out << YAML::Key << "CameraComponent";//������
            out << YAML::BeginMap;//��ʼӳ��

            auto& cameraComponent = entity.GetComponent<CameraComponent>();//��ȡ������
            auto& camera = cameraComponent.Camera;//���

            out << YAML::Key << "Camera" << YAML::Value;//���
            out << YAML::BeginMap;//��ʼӳ��
            out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();//ͶӰ����
            out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();//͸�Ӵ�ֱ�ӽ�
            out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();//͸�ӽ��ü���
            out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();//͸��Զ�ü���
            out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();//������С
            out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();//�������ü���
            out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();//����Զ�ü���
            out << YAML::EndMap;//����ӳ��

            out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;//�����
            out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;//�̶��ݺ��

            out << YAML::EndMap;//����ӳ��
        }

        if (entity.HasComponent<SpriteRendererComponent>())//����о�����Ⱦ�����
        {
            out << YAML::Key << "SpriteRendererComponent";
            out << YAML::BeginMap;//��ʼӳ��

            auto& src = entity.GetComponent<SpriteRendererComponent>();//������Ⱦ�����
            out << YAML::Key << "Color" << YAML::Value << src.Color;//��ɫ

            out << YAML::EndMap;//����ӳ��
        }

        if (entity.HasComponent<CircleRendererComponent>())//�����Բ����Ⱦ�����
        {
            out << YAML::Key << "CircleRendererComponent";
            out << YAML::BeginMap; // CircleRendererComponent

            auto& circleRendererComponent = entity.GetComponent<CircleRendererComponent>();
            out << YAML::Key << "Color" << YAML::Value << circleRendererComponent.Color;
            out << YAML::Key << "Thickness" << YAML::Value << circleRendererComponent.Thickness;
            out << YAML::Key << "Fade" << YAML::Value << circleRendererComponent.Fade;

            out << YAML::EndMap; // CircleRendererComponent
        }

        if (entity.HasComponent<Rigidbody2DComponent>())//����и������
        {
            out << YAML::Key << "Rigidbody2DComponent";
            out << YAML::BeginMap; // Rigidbody2DComponent

            auto& rb2dComponent = entity.GetComponent<Rigidbody2DComponent>();
            out << YAML::Key << "BodyType" << YAML::Value << RigidBody2DBodyTypeToString(rb2dComponent.Type);
            out << YAML::Key << "FixedRotation" << YAML::Value << rb2dComponent.FixedRotation;

            out << YAML::EndMap; // Rigidbody2DComponent
        }

        if (entity.HasComponent<BoxCollider2DComponent>())//����и�����ײ���
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

        out << YAML::EndMap;//����ӳ��
    }

    void SceneSerializer::Serialize(const std::string& filepath)
    {
        YAML::Emitter out;//���
        out << YAML::BeginMap;//��ʼӳ��

        out << YAML::Key << "Scene" << YAML::Value << "Untitled";//����
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;//ʵ��

        m_Scene->m_Registry.each([&](auto entityID)//����ʵ��
        {
            Entity entity = { entityID, m_Scene.get() };//ʵ��
            if (!entity)//���ʵ����Ч
                return;

            SerializeEntity(out, entity);//���л�ʵ��
        });

        out << YAML::EndSeq;//��������
        out << YAML::EndMap;//����ӳ��

        std::ofstream fout(filepath);//����ļ���
        fout << out.c_str();//���
    }

    void SceneSerializer::SerializeRuntime(const std::string& filepath)
    {
        //�޷����л�����ʱ����
        HZ_CORE_ASSERT(false);//����ʧ��
    }

    // �����л�����
    bool SceneSerializer::Deserialize(const std::string& filepath)
    {
        YAML::Node data;// ����
        try
        {
            data = YAML::LoadFile(filepath);//�����ļ�
        }
        catch (YAML::ParserException e)//�����쳣
        {
            HZ_CORE_ERROR("YAML::ParserException: {0}", e.what());//����
            return false;//����ʧ��
        }
        if (!data["Scene"])//���û�г���
            return false;

        std::string sceneName = data["Scene"].as<std::string>();//������
        HZ_CORE_TRACE("Deserializing scene '{0}'", sceneName);//����

        auto entities = data["Entities"];//ʵ��
        if (entities)
        {
            for (auto entity : entities)
            {
                uint64_t uuid = entity["Entity"].as<uint64_t>();//ʵ��ID

                std::string name;//����
                auto tagComponent = entity["TagComponent"];//��ǩ���
                if (tagComponent)
                    name = tagComponent["Tag"].as<std::string>();//����

                HZ_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);//����

                Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);//����ʵ��

                auto transformComponent = entity["TransformComponent"];//�任���
                if (transformComponent)
                {
                    // Entities always have transforms
                    auto& tc = deserializedEntity.GetComponent<TransformComponent>();//�任���
                    tc.Translation = transformComponent["Translation"].as<glm::vec3>();//ƽ��
                    tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();//��ת
                    tc.Scale = transformComponent["Scale"].as<glm::vec3>();//����
                }

                auto cameraComponent = entity["CameraComponent"];//������
                if (cameraComponent)
                {
                    auto& cc = deserializedEntity.AddComponent<CameraComponent>();//������

                    auto& cameraProps = cameraComponent["Camera"];//�������
                    cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());//ͶӰ����

                    cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());//͸�Ӵ�ֱ�ӽ�
                    cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());//͸�ӽ��ü���
                    cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());//͸��Զ�ü���

                    cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());//������С
                    cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());//�������ü���
                    cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());//����Զ�ü���

                    cc.Primary = cameraComponent["Primary"].as<bool>();//�����
                    cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();//�̶��ݺ��
                }

                auto spriteRendererComponent = entity["SpriteRendererComponent"];//������Ⱦ�����
                if (spriteRendererComponent)
                {
                    auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();//������Ⱦ�����
                    src.Color = spriteRendererComponent["Color"].as<glm::vec4>();//��ɫ
                }

                auto circleRendererComponent = entity["CircleRendererComponent"];//Բ����Ⱦ�����
                if (circleRendererComponent)
                {
                    auto& crc = deserializedEntity.AddComponent<CircleRendererComponent>();
                    crc.Color = circleRendererComponent["Color"].as<glm::vec4>();
                    crc.Thickness = circleRendererComponent["Thickness"].as<float>();
                    crc.Fade = circleRendererComponent["Fade"].as<float>();
                }

                auto rigidbody2DComponent = entity["Rigidbody2DComponent"];//�������
                if (rigidbody2DComponent)
                {
                    auto& rb2d = deserializedEntity.AddComponent<Rigidbody2DComponent>();
                    rb2d.Type = RigidBody2DBodyTypeFromString(rigidbody2DComponent["BodyType"].as<std::string>());
                    rb2d.FixedRotation = rigidbody2DComponent["FixedRotation"].as<bool>();
                }

                auto boxCollider2DComponent = entity["BoxCollider2DComponent"];//������ײ���
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
        HZ_CORE_ASSERT(false);//����ʧ��
        return false;//����ʧ��
    }
    
}