#include "hzpch.h"
#include "Scene.h"

#include "Components.h"
#include "Hazel/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

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

    entt::entity Scene::CreateEntity()
    {
        return m_Registry.create();
    }

    void Scene::OnUpdate(Timestep ts)
    {
        auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group)
        {
            auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

            Renderer2D::DrawQuad(transform, sprite.Color);
        }

    }

}