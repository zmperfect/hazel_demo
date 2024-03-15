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

    static b2BodyType Rigidbody2DTypeToBox2DBody(Rigidbody2DComponent::BodyType bodyType)//��������ת��ΪBox2D��������
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

    // ���Ƴ���
    template<typename Component>
    static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
    {
        auto view = src.view<Component>();// ��ȡ����ʵ���Component���
        for (auto e : view)//����
        {
            UUID uuid = src.get<IDComponent>(e).ID;//��ȡID
            HZ_CORE_ASSERT(enttMap.find(uuid) != enttMap.end());//����uuid����
            entt::entity dstEnttID = enttMap.at(uuid);//��ȡĿ��ʵ��ID

            auto& component = src.get<Component>(e);//��ȡ���
            dst.emplace_or_replace<Component>(dstEnttID, component);//��ӻ��滻���
        }
    }

    // ���Ƴ���(������)
    template<typename Component>
    static void CopyComponentIfExists(Entity dst, Entity src)
    {
        if (src.HasComponent<Component>())
            dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());//��ӻ��滻���
    }

    // ���Ƴ���
    Ref<Scene> Scene::Copy(Ref<Scene> other)
    {
        Ref<Scene> newScene = CreateRef<Scene>();// �����³���

        // �����ӿڴ�С
        newScene->m_ViewportWidth = other->m_ViewportWidth;
        newScene->m_ViewportHeight = other->m_ViewportHeight;

        // ��ȡʵ��ע���
        auto& srcSceneRegistry = other->m_Registry;
        auto& dstSceneRegistry = newScene->m_Registry;
        std::unordered_map<UUID, entt::entity> enttMap;//�洢UUID��entt::entity��ӳ��

        // �³�������ʵ��
        auto idView = srcSceneRegistry.view<IDComponent>();
        for (auto e : idView)
        {
            UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
            const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
            Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
            enttMap[uuid] = (entt::entity)newEntity;
        }

        // ����ԭ����������������³���
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
        m_PhysicsWorld = new b2World({ 0.0f, -9.8f });//������������

        auto view = m_Registry.view<Rigidbody2DComponent>();//��ȡ����ʵ���Rigidbody2DComponent���
        for (auto e : view)
        {
            Entity entity = { e, this };
            auto& transform = entity.GetComponent<TransformComponent>();//��ȡTransformComponent���
            auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();//��ȡRigidbody2DComponent���

            b2BodyDef bodyDef;//���嶨��
            bodyDef.type = Rigidbody2DTypeToBox2DBody(rb2d.Type);//��������
            bodyDef.position.Set(transform.Translation.x, transform.Translation.y);//λ��
            bodyDef.angle = transform.Rotation.z;//�Ƕ�

            b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);//��������
            body->SetFixedRotation(rb2d.FixedRotation);
            rb2d.RuntimeBody = body;//����ʱ����

            if (entity.HasComponent<BoxCollider2DComponent>())//�����BoxCollider2DComponent���
            {
                auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

                b2PolygonShape boxShape;//�������״
                boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y);//����Ϊ����

                b2FixtureDef fixtureDef;//�о߶���
                fixtureDef.shape = &boxShape;//��״
                fixtureDef.density = bc2d.Density;//�ܶ�
                fixtureDef.friction = bc2d.Friction;//Ħ����
                fixtureDef.restitution = bc2d.Restitution;//�ָ���
                fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;//�ָ�����ֵ
                body->CreateFixture(&fixtureDef);//�����о�
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
            const int32_t velocityIterations = 6;//�ٶȵ�������
            const int32_t positionIterations = 2;//λ�õ�������
            m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);//�����������

            // ��Box2D����Transform
            auto view = m_Registry.view<Rigidbody2DComponent>();
            for (auto e : view)
            {
                Entity entity = { e, this };
                auto& transform = entity.GetComponent<TransformComponent>();
                auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

                b2Body* body = (b2Body*)rb2d.RuntimeBody;
                const auto& position = body->GetPosition();//��������ʱλ��

                // ����Transform
                transform.Translation.x = position.x;
                transform.Translation.y = position.y;
                transform.Rotation.z = body->GetAngle();
            }
        }

        //Render 2D
        Camera* mainCamera = nullptr;//�����
        glm::mat4 cameraTransform;//���ת������
        {
            auto view = m_Registry.view<TransformComponent, CameraComponent>();//��ȡ����ʵ���TransformComponent��CameraComponent���
            for (auto entity : view)
            {
                auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);//��ȡ���

                if(camera.Primary)//����������
                {
                    mainCamera = &camera.Camera;//��ȡ���
                    cameraTransform = transform.GetTransform();//��ȡ���ת������
                    break;
                }
            }
        }
        
        if (mainCamera)//����������
        {
            Renderer2D::BeginScene(*mainCamera, cameraTransform);//��ʼ��Ⱦ

            auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);//��ȡ����ʵ���TransformComponent��SpriteRendererComponent���
            for (auto entity : group)
            {
                auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);//��ȡ���

                Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);//��Ⱦ
            }

            Renderer2D::EndScene();//������Ⱦ
        }

    }

    void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
    {
        Renderer2D::BeginScene(camera);//��ʼ��Ⱦ

        auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);//��ȡ����ʵ���TransformComponent��SpriteRendererComponent���
        for (auto entity : group)
        {
            auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);//��ȡ���

            Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);//��Ⱦ
        }

        Renderer2D::EndScene();//������Ⱦ
    }

    void Scene::OnViewportResize(uint32_t width, uint32_t height)
    {
        m_ViewportWidth = width;
        m_ViewportHeight = height;

        //Resize non-fixed aspect ratio cameras
        auto view = m_Registry.view<CameraComponent>();//��ȡ����ʵ���CameraComponent���
        for (auto entity : view)
        {
            auto& cameraComponent = view.get<CameraComponent>(entity);//��ȡ���
            if (!cameraComponent.FixedAspectRatio)//������ǹ̶���߱�
            {
                cameraComponent.Camera.SetViewportSize(width, height);//�����ӿڴ�С
            }
        }
    }

    void Scene::DuplicateEntity(Entity entity)
    {
        std::string name = entity.GetName();//��ȡʵ������
        Entity newEntity = CreateEntity(name);//����ʵ��

        CopyComponentIfExists<TransformComponent>(newEntity, entity);//���Ʊ任���
        CopyComponentIfExists<SpriteRendererComponent>(newEntity, entity);//���ƾ�����Ⱦ���
        CopyComponentIfExists<CameraComponent>(newEntity, entity);//����������
        CopyComponentIfExists<NativeScriptComponent>(newEntity, entity);//���Ʊ��ؽű����
        CopyComponentIfExists<Rigidbody2DComponent>(newEntity, entity);//���Ƹ������
        CopyComponentIfExists<BoxCollider2DComponent>(newEntity, entity);//���Ƹ�����ײ���
    }

    Entity Scene::GetPrimaryCameraEntity()
    {
        auto view = m_Registry.view<CameraComponent>();//��ȡ����ʵ���CameraComponent���
        for (auto entity : view)
        {
            const auto& camera = view.get<CameraComponent>(entity);//��ȡ���
            if (camera.Primary)//����������
                return Entity{ entity, this };//����ʵ��
        }
        return {};//���ؿ�ʵ��
    }

    template<typename T>
    void Scene::OnComponentAdded(Entity entity, T& component)
    {
        //static_assert(false);//����
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
            component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);//�����ӿڴ�С
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

    // ��Ӹ������
    template<>
    void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
    {
    }

    // ��Ӹ�����ײ���
    template<>
    void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
    {
    }

}