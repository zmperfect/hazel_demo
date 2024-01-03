#pragma once

#include "entt.hpp"

#include "Hazel/Core/Timestep.h"

namespace Hazel {

    class Entity;

    class Scene
    {
    public:
        Scene();
        ~Scene();

        Entity CreateEntity(const std::string& name = std::string());//����ʵ��
        void DestroyEntity(Entity entity);//����ʵ��

        void OnUpdate(Timestep ts);
        void OnViewportResize(uint32_t width, uint32_t height);//�ӿڵ�����С

        Entity GetPrimaryCameraEntity();//��ȡ�����ʵ��
    private:
        template<typename T>
        void OnComponentAdded(Entity entity, T& component);//�������¼�

        entt::registry m_Registry;//ʵ��ע���
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;//�ӿڿ��

        friend class Entity;//ʵ��
        friend class SceneSerializer;//�������л�
        friend class SceneHierarchyPanel;//�����㼶���
    };
}