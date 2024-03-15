#pragma once

#include "Hazel/Core/Timestep.h"
#include "Hazel/Core/UUID.h"
#include "Hazel/Renderer/EditorCamera.h"

#include "entt.hpp"

class b2World;//物理世界

namespace Hazel {

    class Entity;

    class Scene
    {
    public:
        Scene();
        ~Scene();

        Entity CreateEntity(const std::string& name = std::string());//创建实体
        Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string()); //创建带UUID的实体
        void DestroyEntity(Entity entity);//销毁实体

        void OnRuntimeStart();//运行时开始
        void OnRuntimeStop();//运行时结束

        void OnUpdateRuntime(Timestep ts);//运行时更新
        void OnUpdateEditor(Timestep ts, EditorCamera& camera);//编辑器更新
        void OnViewportResize(uint32_t width, uint32_t height);//视口调整大小

        Entity GetPrimaryCameraEntity();//获取主相机实体
    private:
        template<typename T>
        void OnComponentAdded(Entity entity, T& component);//组件添加事件

        entt::registry m_Registry;//实体注册表
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;//视口宽高

        b2World* m_PhysicsWorld = nullptr;//物理世界

        friend class Entity;//实体
        friend class SceneSerializer;//场景序列化
        friend class SceneHierarchyPanel;//场景层级面板
    };
}