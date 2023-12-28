#include "hzpch.h"
#include "SceneSerializer.h"

#include "Entity.h"
#include "Components.h"

#include <fstream>

#include <yaml-cpp/yaml.h>

namespace YAML {

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

    SceneSerializer::SceneSerializer(const Ref<Scene>& scene)//���캯��
        : m_Scene(scene)
    {
    }

    static void SerializeEntity(YAML::Emitter& out, Entity entity)//���л�ʵ��
    {
        out << YAML::BeginMap;//��ʼӳ��

        out << YAML::Key << "Entity" << YAML::Value << "12837192831273"; // TODO: Entity ID goes here

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
            out << YAML::Key << "SpriteRendererComponent";//������Ⱦ�����
            out << YAML::BeginMap;//��ʼӳ��

            auto& src = entity.GetComponent<SpriteRendererComponent>();//������Ⱦ�����
            out << YAML::Key << "Color" << YAML::Value << src.Color;//��ɫ

            out << YAML::EndMap;//����ӳ��
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

    bool SceneSerializer::Deserialize(const std::string& filepath)
    {
        YAML::Node data = YAML::LoadFile(filepath);//���ļ�·����������
        if (!data["Scene"])//���û�г���
            return false;

        std::string sceneName = data["Scene"].as<std::string>();//������
        HZ_CORE_TRACE("Deserializing scene '{0}'", sceneName);//����

        auto entities = data["Entities"];//ʵ��
        if (entities)
        {
            for (auto entity : entities)
            {
                uint64_t uuid = entity["Entity"].as<uint64_t>();//ʵ��ID TODO

                std::string name;//����
                auto tagComponent = entity["TagComponent"];//��ǩ���
                if (tagComponent)
                    name = tagComponent["Tag"].as<std::string>();//����

                HZ_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);//����

                Entity deserializedEntity = m_Scene->CreateEntity(name);//����ʵ��

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