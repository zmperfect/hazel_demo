#include "SceneHierarchyPanel.h"

#include "imgui/imgui.h"

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
        ImGui::End();//��������������
    }

    void SceneHierarchyPanel::DrawEntityNode(Entity entity)
    {
        auto& tag = entity.GetComponent<TagComponent>().Tag;//��ȡ��ǩ���

        ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;//���ýڵ��־
        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());//���ƽڵ�
        if(ImGui::IsItemClicked())//�������˽ڵ�
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

}
