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
            T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);//创建组件<
            m_Scene->OnComponentAdded<T>(*this, component);//调用组件添加事件
            return component;
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

        operator bool() const { return m_EntityHandle != entt::null; }//重载bool类型,判断实体是否为空
        operator entt::entity() const { return m_EntityHandle; }//重载entt::entity类型,获取实体句柄
        operator uint32_t() const { return (uint32_t)m_EntityHandle; }//重载uint32_t类型,获取实体句柄

        bool operator==(const Entity& other) const//重载==
        {
            return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
        }

        bool operator!=(const Entity& other) const//重载!=
        {
            return !(*this == other);
        }
    private:
        entt::entity m_EntityHandle{ entt::null };//实体句柄
        Scene* m_Scene = nullptr;//场景指针
    };
}