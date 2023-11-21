#include "SceneHierarchyPanel.h"

#include "imgui/imgui.h"

#include "Hazel/Scene/Components.h"

namespace Hazel {

    SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene)
    {
        SetContext(scene);//设置上下文
    }

    void SceneHierarchyPanel::SetContext(const Ref<Scene>& scene)
    {
        m_Context = scene;//设置上下文
    }

    void SceneHierarchyPanel::OnImGuiRender()
    {
        ImGui::Begin("Scene Hierarchy");//开始场景层次面板

        m_Context->m_Registry.each([&](auto entityID)//遍历实体
            {
                Entity entity{ entityID , m_Context.get() };//实体
                DrawEntityNode(entity);//绘制实体节点
            });
        ImGui::End();//结束场景层次面板
    }

    void SceneHierarchyPanel::DrawEntityNode(Entity entity)
    {
        auto& tag = entity.GetComponent<TagComponent>().Tag;//获取标签组件

        ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;//设置节点标志
        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());//绘制节点
        if(ImGui::IsItemClicked())//如果点击了节点
        {
            m_SelectionContext = entity;//设置选择上下文
        }

        if (opened)//如果节点打开
        {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;//设置节点标志
            bool opened = ImGui::TreeNodeEx((void*)10000, flags, tag.c_str());//绘制节点
            if (opened)//如果节点打开
                ImGui::TreePop();//关闭节点
            ImGui::TreePop();//关闭节点
        }

    }

}
