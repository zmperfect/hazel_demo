#include "SceneHierarchyPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

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

        //�հ״��Ҽ��˵�
        if (ImGui::BeginPopupContextWindow(0, 1, false))
        {
            if (ImGui::MenuItem("Create Empty Entity"))//�������˴�����ʵ��
            {
                m_Context->CreateEntity("Empty Entity");//������ʵ��
            }

            ImGui::EndPopup();//�����Ҽ��˵�
        }

        ImGui::End();

        ImGui::Begin("Properties");//��ʼ�������
        if (m_SelectionContext)//���ѡ�������Ĳ�Ϊ��
        {
            DrawComponents(m_SelectionContext);//�������

            if (ImGui::Button("Add Component"))//����������������ť
            {
                ImGui::OpenPopup("AddComponent");//������������
            }

            if (ImGui::BeginPopup("AddComponent"))
            {
                if (ImGui::MenuItem("Camera"))//�����������
                {
                        m_SelectionContext.AddComponent<CameraComponent>();//���������
                        ImGui::CloseCurrentPopup();//�رյ�ǰ����
                    }

                if (ImGui::MenuItem("Sprite Renderer"))//�������˾�����Ⱦ��
                {
                        m_SelectionContext.AddComponent<SpriteRendererComponent>();//��Ӿ�����Ⱦ�����
                        ImGui::CloseCurrentPopup();//�رյ�ǰ����
                    }

                    ImGui::EndPopup();//��������������
            }
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

        bool entityDeleted = false;//ʵ���Ƿ�ɾ��
        if (ImGui::BeginPopupContextItem())//����Ҽ�����˽ڵ�
        {
            if (ImGui::MenuItem("Delete Entity"))//��������ɾ��ʵ��
            {
                entityDeleted = true;//ʵ�屻ɾ��
            }

            ImGui::EndPopup();//�����Ҽ��˵�
        }

        if (opened)//����ڵ��
        {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;//���ýڵ��־
            bool opened = ImGui::TreeNodeEx((void*)10000, flags, tag.c_str());//���ƽڵ�
            if (opened)//����ڵ��
                ImGui::TreePop();//�رսڵ�
            ImGui::TreePop();//�رսڵ�
        }

        if (entityDeleted)//���ʵ�屻ɾ��
        {
            m_Context->DestroyEntity(entity);//����ʵ��
            if (m_SelectionContext == entity)//���ѡ����������ʵ��
                m_SelectionContext = {};//���ѡ��������
        }

    }

    static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
    {
        ImGui::PushID(label.c_str());//ѹ��ID

        ImGui::Columns(2);//��������
        ImGui::SetColumnWidth(0, columnWidth);//�����п�
        ImGui::Text(label.c_str());//�ı�
        ImGui::NextColumn();//��һ��

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());//ѹ�������
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });//ѹ����ʽ����

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;//�и�
        ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };//��ť��С

        //X����ʽ
        ////ѹ�밴ť��ʽ
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        if (ImGui::Button("X", buttonSize))
            values.x = resetValue;
        ImGui::PopStyleColor(3);//������ʽ

        ImGui::SameLine();//ͬһ��
        ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");//��ק������
        ImGui::PopItemWidth();//�������
        ImGui::SameLine();//ͬһ��

        //Y����ʽ
        ////ѹ�밴ť��ʽ
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        if (ImGui::Button("Y", buttonSize))
            values.x = resetValue;
        ImGui::PopStyleColor(3);//������ʽ

        ImGui::SameLine();//ͬһ��
        ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");//��ק������
        ImGui::PopItemWidth();//�������
        ImGui::SameLine();//ͬһ��

        //Z����ʽ
        ////ѹ�밴ť��ʽ
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        if (ImGui::Button("Z", buttonSize))
            values.x = resetValue;
        ImGui::PopStyleColor(3);//������ʽ

        ImGui::SameLine();//ͬһ��
        ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");//��ק������
        ImGui::PopItemWidth();//�������
        ImGui::SameLine();//ͬһ��

        ImGui::PopStyleVar();//������ʽ����

        ImGui::Columns(1);//��������

        ImGui::PopID();//����ID
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

        const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;//�ڵ��־

        if (entity.HasComponent<TransformComponent>())
        {
            bool open = ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), treeNodeFlags, "Transform");//�ڵ�

            if (open)//����ڵ��
            {
                auto& tc = entity.GetComponent<TransformComponent>();//��ȡ�任���
                DrawVec3Control("Translation", tc.Translation);//���������ؼ�
                glm::vec3 rotation = glm::degrees(tc.Rotation);//��תת��Ϊ�Ƕ�
                DrawVec3Control("Rotation", rotation);//���������ؼ�
                tc.Rotation = glm::radians(rotation);//�Ƕ�ת��Ϊ��ת
                DrawVec3Control("Scale", tc.Scale, 1.0f);//���������ؼ�

                ImGui::TreePop();//�رսڵ�
            }
        }

        if (entity.HasComponent<CameraComponent>())//���ʵ����������
        {
            if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), treeNodeFlags, "Camera"))//����ڵ��
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
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
            bool open = ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(), treeNodeFlags, "Sprite Renderer");//�ڵ�
            ImGui::SameLine(ImGui::GetWindowWidth() - 25.0f);
            if (ImGui::Button("+", ImVec2{ 20, 20 }))//��ť
            {
                ImGui::OpenPopup("ComponentSettings");//�򿪵�������
            }
            ImGui::PopStyleVar();//������ʽ����

            bool removeComponent = false;//�Ƿ��Ƴ����
            if (ImGui::BeginPopup("ComponentSettings"))//����������ڴ�
            {
                if (ImGui::MenuItem("Remove component"))//�˵���
                {
                    removeComponent = true;//�Ƴ����
                }

                ImGui::EndPopup();//�رյ�������
            }

            if (open)//����ڵ��
            {
                auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();//��ȡ������Ⱦ�����
                ImGui::ColorEdit4("Color", glm::value_ptr(spriteRendererComponent.Color));//��ɫ�༭��
                ImGui::TreePop();//�رսڵ�
            }

            if (removeComponent)//����Ƴ����
            {
                entity.RemoveComponent<SpriteRendererComponent>();//�Ƴ�������Ⱦ�����
            }
        }
    }

}
