#include "SceneHierarchyPanel.h"

#include "imgui/imgui.h"

#include <glm/gtc/type_ptr.hpp>

#include "Hazel/Scene/Components.h"

namespace Hazel {

    SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene)
    {
        SetContext(scene);//����������
    }

    void SceneHierarchyPanel::SetContext(const Ref<Scene>& scene)
    {
        m_Context = scene;//����������
    }

    void SceneHierarchyPanel::OnImGuiRender()
    {
        ImGui::Begin("Scene Hierarchy");//��ʼ����������

        m_Context->m_Registry.each([&](auto entityID)//����ʵ��
            {
                Entity entity{ entityID , m_Context.get() };//ʵ��
                DrawEntityNode(entity);//����ʵ��ڵ�
            });

        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())//������������²�������ڴ�����
        {
            m_SelectionContext = {};//���ѡ��������
        }

        ImGui::End();

        ImGui::Begin("Properties");//��ʼ�������
        if (m_SelectionContext)//���ѡ�������Ĳ�Ϊ��
        {
            DrawComponents(m_SelectionContext);//�������
        }

        ImGui::End();//��������������
    }

    void SceneHierarchyPanel::DrawEntityNode(Entity entity)
    {
        auto& tag = entity.GetComponent<TagComponent>().Tag;//��ȡ��ǩ���

        ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;//���ýڵ��־
        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());//���ƽڵ�
        if (ImGui::IsItemClicked())//�������˽ڵ�
        {
            m_SelectionContext = entity;//����ѡ��������
        }

        if (opened)//����ڵ��
        {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;//���ýڵ��־
            bool opened = ImGui::TreeNodeEx((void*)10000, flags, tag.c_str());//���ƽڵ�
            if (opened)//����ڵ��
                ImGui::TreePop();//�رսڵ�
            ImGui::TreePop();//�رսڵ�
        }

    }

    void SceneHierarchyPanel::DrawComponents(Entity entity)
    {
        if (entity.HasComponent<TagComponent>())//���ʵ���б�ǩ���
        {
            auto& tag = entity.GetComponent<TagComponent>().Tag;//��ȡ��ǩ���

            char buffer[256];//������
            memset(buffer, 0, sizeof(buffer));//��ջ�����
            strcpy_s(buffer, sizeof(buffer), tag.c_str());//���Ʊ�ǩ��������
            if (ImGui::InputText("Tag", buffer, sizeof(buffer)))//��������ǩ
            {
                tag = std::string(buffer);//���ñ�ǩ
            }
        }

        if (entity.HasComponent<TransformComponent>())
        {
            if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))//����ڵ��
            {
                auto& transform = entity.GetComponent<TransformComponent>().Transform;//��ȡ�任���

                ImGui::DragFloat3("Position", glm::value_ptr(transform[3]), 0.1f);//��קλ��
                ImGui::DragFloat3("Rotation", glm::value_ptr(transform[3]), 0.1f);//��ק��ת
                ImGui::DragFloat3("Scale", glm::value_ptr(transform[3]), 0.1f);//��ק����

                ImGui::TreePop();//�رսڵ�
            }
        }

        if (entity.HasComponent<CameraComponent>())//���ʵ����������
        {
            if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))//����ڵ��
            {
                auto& cameraComponent = entity.GetComponent<CameraComponent>();//��ȡ������
                auto& camera = cameraComponent.Camera;//��ȡ���

                ImGui::Checkbox("Primary", &cameraComponent.Primary);//��ѡ��

                const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };//ͶӰ�����ַ���
                const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];//��ǰͶӰ�����ַ���
                if (ImGui::BeginCombo("Projection", currentProjectionTypeString))//ͶӰ����������
                {
                    for (int i = 0; i < 2; i++)//����ͶӰ����
                    {
                        bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];//�Ƿ�ѡ��
                        if (ImGui::Selectable(projectionTypeStrings[i], isSelected))//ѡ��ͶӰ����
                        {
                            currentProjectionTypeString = projectionTypeStrings[i];//���õ�ǰͶӰ�����ַ���
                            camera.SetProjectionType((SceneCamera::ProjectionType)i);//����ͶӰ����
                        }

                        if (isSelected)//���ѡ��
                        {
                            ImGui::SetItemDefaultFocus();//����Ĭ�Ͻ���
                        }
                    }

                    ImGui::EndCombo();//����ͶӰ����������
                }

                if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)//�����͸��ͶӰ
                {
                    float perspectiveVerticalFov = glm::degrees(camera.GetPerspectiveVerticalFOV());//��ȡ͸�Ӵ�ֱ�ӽ�
                    if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFov))//��ק͸�Ӵ�ֱ�ӽ�
                    {
                        camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));//����͸�Ӵ�ֱ�ӽ�
                    }

                    float perspectiveOrthoNear = camera.GetPerspectiveNearClip();//��ȡ͸�ӽ��ü���
                    if (ImGui::DragFloat("Near", &perspectiveOrthoNear))//��ק͸�ӽ��ü���
                    {
                        camera.SetPerspectiveNearClip(perspectiveOrthoNear);//����͸�ӽ��ü���
                    }

                    float perspectiveOrthoFar = camera.GetPerspectiveFarClip();//��ȡ͸��Զ�ü���
                    if (ImGui::DragFloat("Far", &perspectiveOrthoFar))//��ק͸��Զ�ü���
                    {
                        camera.SetPerspectiveFarClip(perspectiveOrthoFar);//����͸��Զ�ü���
                    }
                }

                if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
                {
                    float orthoSize = camera.GetOrthographicSize();//��ȡ������С
                    if (ImGui::DragFloat("Size", &orthoSize))//��ק������С
                    {
                        camera.SetOrthographicSize(orthoSize);//����������С
                    }

                    float orthoNear = camera.GetOrthographicNearClip();//��ȡ�������ü���
                    if (ImGui::DragFloat("Near", &orthoNear))//��ק�������ü���
                    {
                        camera.SetOrthographicNearClip(orthoNear);//�����������ü���
                    }

                    float orthoFar = camera.GetOrthographicFarClip();//��ȡ����Զ�ü���
                    if (ImGui::DragFloat("Far", &orthoFar))//��ק����Զ�ü���
                    {
                        camera.SetOrthographicFarClip(orthoFar);//��������Զ�ü���
                    }

                    ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.FixedAspectRatio);//��ѡ��
                }

                ImGui::TreePop();//�رսڵ�
            }
        }

        if (entity.HasComponent<SpriteRendererComponent>())//���ʵ���о�����Ⱦ�����
        {
            if (ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Sprite Renderer"))//����ڵ��
            {
                auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();//��ȡ������Ⱦ�����
                ImGui::ColorEdit4("Color", glm::value_ptr(spriteRendererComponent.Color));//��ɫ�༭��
                ImGui::TreePop();//�رսڵ�
            }
        }
    }

}
