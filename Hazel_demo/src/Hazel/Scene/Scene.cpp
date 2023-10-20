#include "hzpch.h"
#include "Scene.h"

#include "Components.h"
#include "Hazel/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

#include "Entity.h"

namespace Hazel {

    static void DoMath(const glm::mat4& transform)
    {

    }

    static void OntransformConstruct(entt::registry& registry, entt::entity entity)
    {

    }

    Scene::Scene()
    {
#if ENTT_EXAMPLE_CODE
        entt::entity entity = m_Registry.create();//创建实体
        m_Registry.emplace<TransformComponent>(entity);//添加组件

        m_Registry.on_construct<TransformComponent>().connect<&OntransformConstruct>();//注册回调函数

        if (m_Registry.has<TransformComponent>(entity))//检查实体是否有组件
            TransformComponent& transform = m_Registry.get<TransformComponent>(entity);//获取组件

        auto view = m_Registry.view<TransformComponent>();//获取所有实体的TransformComponent组件
        for (auto entity : view)
        {
            TransformComponent& transform = view.get<TransformComponent>(entity);
        }

        auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);//获取所有实体的TransformComponent和MeshComponent组件
        for (auto entity : group)
        {
            auto& [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
        }
#endif
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

    void Scene::OnUpdate(Timestep ts)
    {
        //Render 2D
        Camera* mainCamera = nullptr;//主相机
        glm::mat4* cameraTransform = nullptr;//相机转换矩阵
        {
            auto group = m_Registry.view<TransformComponent, CameraComponent>();//获取所有实体的TransformComponent和CameraComponent组件
            for (auto entity : group)
            {
                auto& [transform, camera] = group.get<TransformComponent, CameraComponent>(entity);//获取组件

                if(camera.Primary)//如果是主相机
                {
                    mainCamera = &camera.Camera;//获取相机
                    cameraTransform = &transform.Transform;//获取相机转换矩阵
                    break;
                }
            }
        }
        
        if (mainCamera)//如果有主相机
        {
            Renderer2D::BeginScene(mainCamera->GetProjection(), *cameraTransform);//开始渲染

            auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);//获取所有实体的TransformComponent和SpriteRendererComponent组件
            for (auto entity : group)
            {
                auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);//获取组件

                Renderer2D::DrawQuad(transform, sprite.Color);//渲染
            }

            Renderer2D::EndScene();//结束渲染
        }

    }

}