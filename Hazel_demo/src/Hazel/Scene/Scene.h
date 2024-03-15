#pragma once

#include "Hazel/Core/Timestep.h"
#include "Hazel/Core/UUID.h"
#include "Hazel/Renderer/EditorCamera.h"

#include "entt.hpp"

class b2World;//��������

namespace Hazel {

    class Entity;

    class Scene
    {
    public:
        Scene();
        ~Scene();

        Entity CreateEntity(const std::string& name = std::string());//����ʵ��
        Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string()); //������UUID��ʵ��
        void DestroyEntity(Entity entity);//����ʵ��

        void OnRuntimeStart();//����ʱ��ʼ
        void OnRuntimeStop();//����ʱ����

        void OnUpdateRuntime(Timestep ts);//����ʱ����
        void OnUpdateEditor(Timestep ts, EditorCamera& camera);//�༭������
        void OnViewportResize(uint32_t width, uint32_t height);//�ӿڵ�����С

        Entity GetPrimaryCameraEntity();//��ȡ�����ʵ��
    private:
        template<typename T>
        void OnComponentAdded(Entity entity, T& component);//�������¼�

        entt::registry m_Registry;//ʵ��ע���
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;//�ӿڿ��

        b2World* m_PhysicsWorld = nullptr;//��������

        friend class Entity;//ʵ��
        friend class SceneSerializer;//�������л�
        friend class SceneHierarchyPanel;//�����㼶���
    };
}