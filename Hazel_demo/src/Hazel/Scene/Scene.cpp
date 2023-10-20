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
        entt::entity entity = m_Registry.create();//����ʵ��
        m_Registry.emplace<TransformComponent>(entity);//������

        m_Registry.on_construct<TransformComponent>().connect<&OntransformConstruct>();//ע��ص�����

        if (m_Registry.has<TransformComponent>(entity))//���ʵ���Ƿ������
            TransformComponent& transform = m_Registry.get<TransformComponent>(entity);//��ȡ���

        auto view = m_Registry.view<TransformComponent>();//��ȡ����ʵ���TransformComponent���
        for (auto entity : view)
        {
            TransformComponent& transform = view.get<TransformComponent>(entity);
        }

        auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);//��ȡ����ʵ���TransformComponent��MeshComponent���
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
        Camera* mainCamera = nullptr;//�����
        glm::mat4* cameraTransform = nullptr;//���ת������
        {
            auto group = m_Registry.view<TransformComponent, CameraComponent>();//��ȡ����ʵ���TransformComponent��CameraComponent���
            for (auto entity : group)
            {
                auto& [transform, camera] = group.get<TransformComponent, CameraComponent>(entity);//��ȡ���

                if(camera.Primary)//����������
                {
                    mainCamera = &camera.Camera;//��ȡ���
                    cameraTransform = &transform.Transform;//��ȡ���ת������
                    break;
                }
            }
        }
        
        if (mainCamera)//����������
        {
            Renderer2D::BeginScene(mainCamera->GetProjection(), *cameraTransform);//��ʼ��Ⱦ

            auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);//��ȡ����ʵ���TransformComponent��SpriteRendererComponent���
            for (auto entity : group)
            {
                auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);//��ȡ���

                Renderer2D::DrawQuad(transform, sprite.Color);//��Ⱦ
            }

            Renderer2D::EndScene();//������Ⱦ
        }

    }

}