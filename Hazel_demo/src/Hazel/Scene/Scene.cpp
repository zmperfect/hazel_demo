#include "hzpch.h"
#include "Scene.h"

#include "Components.h"
#include "Hazel/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

#include "Entity.h"

namespace Hazel {

    Scene::Scene()
    {
    }

    Scene::~Scene()
    {
    }

    Entity Scene::CreateEntity(const std::string& name)
    {
        Entity entity = { m_Registry.create(), this };
        entity.AddComponent<TransformComponent>();
        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;
        return entity;
    }

    void Scene::DestroyEntity(Entity entity)
    {
        m_Registry.destroy(entity);
    }

    void Scene::OnUpdateRuntime(Timestep ts)
    {
        //Update Scripts
        {
            m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
                {
                    // TODO: Move to Scene::OnScenePlay
                    if (!nsc.Instance)
                    {
                        nsc.Instance = nsc.InstantiateScript();
                        nsc.Instance->m_Entity = Entity{ entity, this };

                        nsc.Instance->OnCreate();
                    }

                    nsc.Instance->OnUpdate(ts);
                });
        }

        //Render 2D
        Camera* mainCamera = nullptr;//主相机
        glm::mat4 cameraTransform;//相机转换矩阵
        {
            auto view = m_Registry.view<TransformComponent, CameraComponent>();//获取所有实体的TransformComponent和CameraComponent组件
            for (auto entity : view)
            {
                auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);//获取组件

                if(camera.Primary)//如果是主相机
                {
                    mainCamera = &camera.Camera;//获取相机
                    cameraTransform = transform.GetTransform();//获取相机转换矩阵
                    break;
                }
            }
        }
        
        if (mainCamera)//如果有主相机
        {
            Renderer2D::BeginScene(*mainCamera, cameraTransform);//开始渲染

            auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);//获取所有实体的TransformComponent和SpriteRendererComponent组件
            for (auto entity : group)
            {
                auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);//获取组件

                Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);//渲染
            }

            Renderer2D::EndScene();//结束渲染
        }

    }

    void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
    {
        Renderer2D::BeginScene(camera);//开始渲染

        auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);//获取所有实体的TransformComponent和SpriteRendererComponent组件
        for (auto entity : group)
        {
            auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);//获取组件

            Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);//渲染
        }

        Renderer2D::EndScene();//结束渲染
    }

    void Scene::OnViewportResize(uint32_t width, uint32_t height)
    {
        m_ViewportWidth = width;
        m_ViewportHeight = height;

        //Resize non-fixed aspect ratio cameras
        auto view = m_Registry.view<CameraComponent>();//获取所有实体的CameraComponent组件
        for (auto entity : view)
        {
            auto& cameraComponent = view.get<CameraComponent>(entity);//获取组件
            if (!cameraComponent.FixedAspectRatio)//如果不是固定宽高比
            {
                cameraComponent.Camera.SetViewportSize(width, height);//设置视口大小
            }
        }
    }

    Entity Scene::GetPrimaryCameraEntity()
    {
        auto view = m_Registry.view<CameraComponent>();//获取所有实体的CameraComponent组件
        for (auto entity : view)
        {
            const auto& camera = view.get<CameraComponent>(entity);//获取组件
            if (camera.Primary)//如果是主相机
                return Entity{ entity, this };//返回实体
        }
        return {};//返回空实体
    }

    template<typename T>
    void Scene::OnComponentAdded(Entity entity, T& component)
    {
        static_assert(false);//断言
    }

    template<>
    void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
    {
        if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
            component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);//设置视口大小
    }

    template<>
    void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
    {
    }

}