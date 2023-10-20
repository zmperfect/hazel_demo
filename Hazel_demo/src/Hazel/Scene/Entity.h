#pragma once

#include "Scene.h"

#include "entt.hpp"

namespace Hazel {

    class Entity
    {
    public:
        Entity() = default;
        Entity(entt::entity handle, Scene* scene);//实体句柄，场景指针
        Entity(const Entity& other) = default;

        template<typename T, typename... Args> //模板参数包
        T& AddComponent(Args&&... args)//参数包展开
        {
            HZ_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
            return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);//将组件添加到实体
        }

        template<typename T>
        T& GetComponent()
        {
            HZ_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
            return m_Scene->m_Registry.get<T>(m_EntityHandle);//获取组件
        }

        template<typename T>
        bool HasComponent()
        {
            return m_Scene->m_Registry.has<T>(m_EntityHandle);//判断实体是否有组件
        }

        template<typename T>
        void RemoveComponent()
        {
            HZ_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
            m_Scene->m_Registry.remove<T>(m_EntityHandle);//移除组件
        }

        operator bool() const { return m_EntityHandle != entt::null; }//重载bool类型
    private:
        entt::entity m_EntityHandle{ entt::null };//实体句柄
        Scene* m_Scene = nullptr;//场景指针
    };
}