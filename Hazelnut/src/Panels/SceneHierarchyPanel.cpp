#include "SceneHierarchyPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

#include "Hazel/Scene/Components.h"
#include <cstring>

//Microsoft C++��������֧��C++��׼�⣬��Ҫһ�µĶ��������Թ���std::strncpy()�ľ���

#ifdef _MSVC_LANG   //�����Microsoft C++������
    #define _CRT_SECURE_NO_WARNINGS//���Թ���std::strncpy()�ľ���
#endif

namespace Hazel {

    extern const std::filesystem::path g_AssetPath;

    SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene)
    {
        SetContext(scene);//����������
    }

    void SceneHierarchyPanel::SetContext(const Ref<Scene>& scene)
    {
        m_Context = scene;//����������
        m_SelectionContext = {};//���ѡ��������
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
        }

        ImGui::End();//��������������
    }

    void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
    {
        m_SelectionContext = entity;//����ѡ��������
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
        ImGuiIO& io = ImGui::GetIO();//��ȡIO
        auto boldFont = io.Fonts->Fonts[0];//��ȡ��������

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
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });//ѹ�밴ť��ʽ
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });//ѹ�밴ť��ͣ��ʽ
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });//ѹ�밴ť������ʽ
        ImGui::PushFont(boldFont);//ѹ���������
        if (ImGui::Button("X", buttonSize))
            values.x = resetValue;
        ImGui::PopFont();//��������
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
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Y", buttonSize))
            values.x = resetValue;
        ImGui::PopFont();
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

    template<typename T, typename UIFunction>
    static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)//�������
    {
        const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;//�ڵ��־,Ĭ�ϴ�,�����,�����ص�,�����

        if (entity.HasComponent<T>())//���ʵ�������
        {
            auto& component = entity.GetComponent<T>();//��ȡ���
            ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();//��ȡ����������ô�С

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });//ѹ����ʽ����
            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;//�и�
            ImGui::Separator();//�ָ���
            bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());//�ڵ�
            ImGui::PopStyleVar();//������ʽ����
            ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);//ͬһ��
            if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))//��ť
            {
                ImGui::OpenPopup("ComponentSettings");//�򿪵�������
            }

            bool removeComponent = false;//�Ƿ��Ƴ����
            if (ImGui::BeginPopup("ComponentSettings"))//����������ڴ�
            {
                if (ImGui::MenuItem("Remove Component"))//���������Ƴ����
                    entity.RemoveComponent<T>();//�Ƴ����

                ImGui::EndPopup();//������������
            }

            if (open)//����ڵ��
            {
                uiFunction(component);//UI����
                ImGui::TreePop();//�رսڵ�
            }

            if (removeComponent)//����Ƴ����
                entity.RemoveComponent<T>();//�Ƴ����
        }
    }

    void SceneHierarchyPanel::DrawComponents(Entity entity)
    {
        if (entity.HasComponent<TagComponent>())//���ʵ���б�ǩ���
        {
            auto& tag = entity.GetComponent<TagComponent>().Tag;//��ȡ��ǩ���

            char buffer[256];//������
            memset(buffer, 0, sizeof(buffer));//��ջ�����
            std::strncpy(buffer, tag.c_str(), sizeof(buffer));//���Ʊ�ǩ��������
            if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))//��������ǩ��##Tag��ʾ����ʾ��ǩ
            {
                tag = std::string(buffer);//���ñ�ǩ
            }
        }

        ImGui::SameLine();//ͬһ��
        ImGui::PushItemWidth(-1);//ѹ����

        if (ImGui::Button("Add Component"))//����������������ť
        {
            ImGui::OpenPopup("AddComponent");//�򿪵�������
        }

        if (ImGui::BeginPopup("AddComponent"))//����������ڴ�
        {
            if (ImGui::MenuItem("Camera"))//�����������
            {
                if (!m_SelectionContext.HasComponent<CameraComponent>())//���ѡ���ʵ��û��������
                    m_SelectionContext.AddComponent<CameraComponent>();//���������
                else
                    HZ_CORE_WARN("This entity already has a camera component!");
                ImGui::CloseCurrentPopup();//�رյ�ǰ��������
            }

            if (ImGui::MenuItem("Sprite Renderer"))//�������˾�����Ⱦ��
            {
                if (!m_SelectionContext.HasComponent<SpriteRendererComponent>())//���ѡ���ʵ��û�о�����Ⱦ�����
                    m_SelectionContext.AddComponent<SpriteRendererComponent>();//��Ӿ�����Ⱦ�����
                else
                    HZ_CORE_WARN("This entity already has a sprite renderer component!");
                ImGui::CloseCurrentPopup();//�رյ�ǰ��������
            }

            ImGui::EndPopup();//������������
        }

        ImGui::PopItemWidth();//�������

        DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
        {
            DrawVec3Control("Translation", component.Translation);//���Ʊ任�ؼ�
            glm::vec3 rotation = glm::degrees(component.Rotation);//��ת�Ƕ�
            DrawVec3Control("Rotation", rotation);//������ת�ؼ�
            component.Rotation = glm::radians(rotation);//��ת��ֵ
            DrawVec3Control("Scale", component.Scale, 1.0f);//�������ſؼ�
        });//���Ʊ任���
        
        DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
        {
            auto& camera = component.Camera;//��ȡ������

            ImGui::Checkbox("Primary", &component.Primary);//�������ѡ��
            const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };//ͶӰ�����ַ���(͸�ӣ�����)
            const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];//��ǰͶӰ�����ַ���
            if (ImGui::BeginCombo("Projection", currentProjectionTypeString))//���ͶӰ�����������
            {
                for (int i = 0; i < 2; i++)//����ͶӰ����
                {
                    bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];//�Ƿ�ѡ��
                    if (ImGui::Selectable(projectionTypeStrings[i], isSelected))//���ѡ����ͶӰ����
                    {
                        currentProjectionTypeString = projectionTypeStrings[i];//���õ�ǰͶӰ����
                        camera.SetProjectionType((SceneCamera::ProjectionType)i);//����ͶӰ����
                    }

                    if (isSelected)//���ѡ��
                        ImGui::SetItemDefaultFocus();//����Ĭ�Ͻ���
                }

                ImGui::EndCombo();//����ͶӰ����������
            }

            if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)//�����͸��ͶӰ
            {
                float perspectiveVerticalFOV = glm::degrees(camera.GetPerspectiveVerticalFOV());//��ֱ�ӽ�
                if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFOV))//��ק������
                    camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFOV));//���ô�ֱ�ӽ�

                float perspectiveNear = camera.GetPerspectiveNearClip();//���ü���
                if (ImGui::DragFloat("Near", &perspectiveNear))//��ק������
                    camera.SetPerspectiveNearClip(perspectiveNear);//���ý��ü���

                float perspectiveFar = camera.GetPerspectiveFarClip();//Զ�ü���
                if (ImGui::DragFloat("Far", &perspectiveFar))//��ק������
                    camera.SetPerspectiveFarClip(perspectiveFar);//����Զ�ü���
            }

            if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)//���������ͶӰ
            {
                float orthographicSize = camera.GetOrthographicSize();//������С
                if (ImGui::DragFloat("Size", &orthographicSize))//��ק������
                    camera.SetOrthographicSize(orthographicSize);//����������С

                float orthographicNear = camera.GetOrthographicNearClip();//���ü���
                if (ImGui::DragFloat("Near", &orthographicNear))//��ק������
                    camera.SetOrthographicNearClip(orthographicNear);//���ý��ü���

                float orthographicFar = camera.GetOrthographicFarClip();//Զ�ü���
                if (ImGui::DragFloat("Far", &orthographicFar))//��ק������
                    camera.SetOrthographicFarClip(orthographicFar);//����Զ�ü���

                ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);//�̶���߱ȸ�ѡ��
            }
        });//����������

        DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
        {
            ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));//��ɫ�༭��

            // Texture����ק����
            ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
					Ref<Texture2D> texture = Texture2D::Create(texturePath.string());
                    if (texture->IsLoaded())
                        component.Texture = texture;//��������
                    else
                        HZ_CORE_WARN("Failed to load texture '{0}'", texturePath.string());
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f);
        });//���ƾ�����Ⱦ�����
    }

}
