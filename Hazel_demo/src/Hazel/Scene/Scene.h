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

        Entity CreateEntity(const std::string& name = std::string());//创建实体
        void DestroyEntity(Entity entity);//销毁实体

        void OnUpdate(Timestep ts);
        void OnViewportResize(uint32_t width, uint32_t height);
    private:
        template<typename T>
        void OnComponentAdded(Entity entity, T& component);//组件添加事件

        entt::registry m_Registry;//实体注册表
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;//视口宽高

        friend class Entity;
        friend class SceneHierarchyPanel;
    };
}