#pragma once

#include "Scene.h"

#include "entt.hpp"

namespace Hazel {

    class Entity
    {
    public:
        Entity() = default;
        Entity(entt::entity handle, Scene* scene);//ʵ����������ָ��
        Entity(const Entity& other) = default;

        template<typename T, typename... Args> //ģ�������
        T& AddComponent(Args&&... args)//������չ��
        {
            HZ_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
            return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);//�������ӵ�ʵ��
        }

        template<typename T>
        T& GetComponent()
        {
            HZ_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
            return m_Scene->m_Registry.get<T>(m_EntityHandle);//��ȡ���
        }

        template<typename T>
        bool HasComponent()
        {
            return m_Scene->m_Registry.has<T>(m_EntityHandle);//�ж�ʵ���Ƿ������
        }

        template<typename T>
        void RemoveComponent()
        {
            HZ_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
            m_Scene->m_Registry.remove<T>(m_EntityHandle);//�Ƴ����
        }

        operator bool() const { return m_EntityHandle != entt::null; }//����bool����
    private:
        entt::entity m_EntityHandle{ entt::null };//ʵ����
        Scene* m_Scene = nullptr;//����ָ��
    };
}