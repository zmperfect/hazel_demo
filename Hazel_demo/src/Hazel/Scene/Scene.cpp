#include "hzpch.h"
#include "Scene.h"

#include "Components.h"
#include "ScriptableEntity.h"
#include "Hazel/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

#include "Entity.h"

// Box2D
#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"

namespace Hazel {

    static b2BodyType Rigidbody2DTypeToBox2DBody(Rigidbody2DComponent::BodyType bodyType)//刚体类型转换为Box2D刚体类型
    {
        switch (bodyType)
        {
        case Rigidbody2DComponent::BodyType::Static:    return b2_staticBody;
        case Rigidbody2DComponent::BodyType::Dynamic:   return b2_dynamicBody;
        case Rigidbody2DComponent::BodyType::Kinematic: return b2_kinematicBody;
        }

        HZ_CORE_ASSERT(false, "Unknown body type");
        return b2_staticBody;
    }

    Scene::Scene()
    {
    }

    Scene::~Scene()
    {
    }

    // 复制场景
    template<typename Component>
    static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
    {
        auto view = src.view<Component>();// 获取所有实体的Component组件
        for (auto e : view)//遍历
        {
            UUID uuid = src.get<IDComponent>(e).ID;//获取ID
            HZ_CORE_ASSERT(enttMap.find(uuid) != enttMap.end());//断言uuid存在
            entt::entity dstEnttID = enttMap.at(uuid);//获取目标实体ID

            auto& component = src.get<Component>(e);//获取组件
            dst.emplace_or_replace<Component>(dstEnttID, component);//添加或替换组件
        }
    }

    // 复制场景(若存在)
    template<typename Component>
    static void CopyComponentIfExists(Entity dst, Entity src)
    {
        if (src.HasComponent<Component>())
            dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());//添加或替换组件
    }

    // 复制场景
    Ref<Scene> Scene::Copy(Ref<Scene> other)
    {
        Ref<Scene> newScene = CreateRef<Scene>();// 创建新场景

        // 复制视口大小
        newScene->m_ViewportWidth = other->m_ViewportWidth;
        newScene->m_ViewportHeight = other->m_ViewportHeight;

        // 获取实体注册表
        auto& srcSceneRegistry = other->m_Registry;
        auto& dstSceneRegistry = newScene->m_Registry;
        std::unordered_map<UUID, entt::entity> enttMap;//存储UUID和entt::entity的映射

        // 新场景创建实体
        auto idView = srcSceneRegistry.view<IDComponent>();
        for (auto e : idView)
        {
            UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
            const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
            Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
            enttMap[uuid] = (entt::entity)newEntity;
        }

        // 复制原场景的所有组件到新场景
        CopyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<SpriteRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<NativeScriptComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<Rigidbody2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<BoxCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);

        return newScene;
    }

    Entity Scene::CreateEntity(const std::string& name)
    {
        return CreateEntityWithUUID(UUID(), name);
    }

    Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
    {
        Entity entity = { m_Registry.create(), this };
        entity.AddComponent<IDComponent>(uuid);
        entity.AddComponent<TransformComponent>();
        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;
        return entity;
    }

    void Scene::DestroyEntity(Entity entity)
    {
        m_Registry.destroy(entity);
    }

    void Scene::OnRuntimeStart()
    {
        m_PhysicsWorld = new b2World({ 0.0f, -9.8f });//创建物理世界

        auto view = m_Registry.view<Rigidbody2DComponent>();//获取所有实体的Rigidbody2DComponent组件
        for (auto e : view)
        {
            Entity entity = { e, this };
            auto& transform = entity.GetComponent<TransformComponent>();//获取TransformComponent组件
            auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();//获取Rigidbody2DComponent组件

            b2BodyDef bodyDef;//刚体定义
            bodyDef.type = Rigidbody2DTypeToBox2DBody(rb2d.Type);//刚体类型
            bodyDef.position.Set(transform.Translation.x, transform.Translation.y);//位置
            bodyDef.angle = transform.Rotation.z;//角度

            b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);//创建刚体
            body->SetFixedRotation(rb2d.FixedRotation);
            rb2d.RuntimeBody = body;//运行时刚体

            if (entity.HasComponent<BoxCollider2DComponent>())//如果有BoxCollider2DComponent组件
            {
                auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

                b2PolygonShape boxShape;//多边形形状
                boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y);//设置为矩形

                b2FixtureDef fixtureDef;//夹具定义
                fixtureDef.shape = &boxShape;//形状
                fixtureDef.density = bc2d.Density;//密度
                fixtureDef.friction = bc2d.Friction;//摩擦力
                fixtureDef.restitution = bc2d.Restitution;//恢复力
                fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;//恢复力阈值
                body->CreateFixture(&fixtureDef);//创建夹具
            }
        }
    }

    void Scene::OnRuntimeStop()
    {
        delete m_PhysicsWorld;
        m_PhysicsWorld = nullptr;
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

        // Physics
        {
            const int32_t velocityIterations = 6;//速度迭代次数
            const int32_t positionIterations = 2;//位置迭代次数
            m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);//物理世界更新

            // 从Box2D更新Transform
            auto view = m_Registry.view<Rigidbody2DComponent>();
            for (auto e : view)
            {
                Entity entity = { e, this };
                auto& transform = entity.GetComponent<TransformComponent>();
                auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

                b2Body* body = (b2Body*)rb2d.RuntimeBody;
                const auto& position = body->GetPosition();//物体运行时位置

                // 更新Transform
                transform.Translation.x = position.x;
                transform.Translation.y = position.y;
                transform.Rotation.z = body->GetAngle();
            }
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

    void Scene::DuplicateEntity(Entity entity)
    {
        std::string name = entity.GetName();//获取实体名称
        Entity newEntity = CreateEntity(name);//创建实体

        CopyComponentIfExists<TransformComponent>(newEntity, entity);//复制变换组件
        CopyComponentIfExists<SpriteRendererComponent>(newEntity, entity);//复制精灵渲染组件
        CopyComponentIfExists<CameraComponent>(newEntity, entity);//复制相机组件
        CopyComponentIfExists<NativeScriptComponent>(newEntity, entity);//复制本地脚本组件
        CopyComponentIfExists<Rigidbody2DComponent>(newEntity, entity);//复制刚体组件
        CopyComponentIfExists<BoxCollider2DComponent>(newEntity, entity);//复制刚体碰撞组件
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
        //static_assert(false);//断言
    }

    template<>
    void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
    {
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

    // 添加刚体组件
    template<>
    void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
    {
    }

    // 添加刚体碰撞组件
    template<>
    void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
    {
    }

}