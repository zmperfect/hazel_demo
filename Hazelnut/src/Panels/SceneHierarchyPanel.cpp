#include "SceneHierarchyPanel.h"

#include "imgui/imgui.h"

#include <glm/gtc/type_ptr.hpp>

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

        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())//如果鼠标左键按下并且鼠标在窗口上
        {
            m_SelectionContext = {};//清空选择上下文
        }

        ImGui::End();

        ImGui::Begin("Properties");//开始属性面板
        if (m_SelectionContext)//如果选择上下文不为空
        {
            DrawComponents(m_SelectionContext);//绘制组件
        }

        ImGui::End();//结束场景层次面板
    }

    void SceneHierarchyPanel::DrawEntityNode(Entity entity)
    {
        auto& tag = entity.GetComponent<TagComponent>().Tag;//获取标签组件

        ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;//设置节点标志
        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());//绘制节点
        if (ImGui::IsItemClicked())//如果点击了节点
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

    void SceneHierarchyPanel::DrawComponents(Entity entity)
    {
        if (entity.HasComponent<TagComponent>())//如果实体有标签组件
        {
            auto& tag = entity.GetComponent<TagComponent>().Tag;//获取标签组件

            char buffer[256];//缓冲区
            memset(buffer, 0, sizeof(buffer));//清空缓冲区
            strcpy_s(buffer, sizeof(buffer), tag.c_str());//复制标签到缓冲区
            if (ImGui::InputText("Tag", buffer, sizeof(buffer)))//如果输入标签
            {
                tag = std::string(buffer);//设置标签
            }
        }

        if (entity.HasComponent<TransformComponent>())
        {
            if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))//如果节点打开
            {
                auto& transform = entity.GetComponent<TransformComponent>().Transform;//获取变换组件

                ImGui::DragFloat3("Position", glm::value_ptr(transform[3]), 0.1f);//拖拽位置
                ImGui::DragFloat3("Rotation", glm::value_ptr(transform[3]), 0.1f);//拖拽旋转
                ImGui::DragFloat3("Scale", glm::value_ptr(transform[3]), 0.1f);//拖拽缩放

                ImGui::TreePop();//关闭节点
            }
        }

        if (entity.HasComponent<CameraComponent>())//如果实体有相机组件
        {
            if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))//如果节点打开
            {
                auto& cameraComponent = entity.GetComponent<CameraComponent>();//获取相机组件
                auto& camera = cameraComponent.Camera;//获取相机

                ImGui::Checkbox("Primary", &cameraComponent.Primary);//复选框

                const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };//投影类型字符串
                const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];//当前投影类型字符串
                if (ImGui::BeginCombo("Projection", currentProjectionTypeString))//投影类型下拉框
                {
                    for (int i = 0; i < 2; i++)//遍历投影类型
                    {
                        bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];//是否选中
                        if (ImGui::Selectable(projectionTypeStrings[i], isSelected))//选择投影类型
                        {
                            currentProjectionTypeString = projectionTypeStrings[i];//设置当前投影类型字符串
                            camera.SetProjectionType((SceneCamera::ProjectionType)i);//设置投影类型
                        }

                        if (isSelected)//如果选中
                        {
                            ImGui::SetItemDefaultFocus();//设置默认焦点
                        }
                    }

                    ImGui::EndCombo();//结束投影类型下拉框
                }

                if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)//如果是透视投影
                {
                    float perspectiveVerticalFov = glm::degrees(camera.GetPerspectiveVerticalFOV());//获取透视垂直视角
                    if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFov))//拖拽透视垂直视角
                    {
                        camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));//设置透视垂直视角
                    }

                    float perspectiveOrthoNear = camera.GetPerspectiveNearClip();//获取透视近裁剪面
                    if (ImGui::DragFloat("Near", &perspectiveOrthoNear))//拖拽透视近裁剪面
                    {
                        camera.SetPerspectiveNearClip(perspectiveOrthoNear);//设置透视近裁剪面
                    }

                    float perspectiveOrthoFar = camera.GetPerspectiveFarClip();//获取透视远裁剪面
                    if (ImGui::DragFloat("Far", &perspectiveOrthoFar))//拖拽透视远裁剪面
                    {
                        camera.SetPerspectiveFarClip(perspectiveOrthoFar);//设置透视远裁剪面
                    }
                }

                if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
                {
                    float orthoSize = camera.GetOrthographicSize();//获取正交大小
                    if (ImGui::DragFloat("Size", &orthoSize))//拖拽正交大小
                    {
                        camera.SetOrthographicSize(orthoSize);//设置正交大小
                    }

                    float orthoNear = camera.GetOrthographicNearClip();//获取正交近裁剪面
                    if (ImGui::DragFloat("Near", &orthoNear))//拖拽正交近裁剪面
                    {
                        camera.SetOrthographicNearClip(orthoNear);//设置正交近裁剪面
                    }

                    float orthoFar = camera.GetOrthographicFarClip();//获取正交远裁剪面
                    if (ImGui::DragFloat("Far", &orthoFar))//拖拽正交远裁剪面
                    {
                        camera.SetOrthographicFarClip(orthoFar);//设置正交远裁剪面
                    }

                    ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.FixedAspectRatio);//复选框
                }

                ImGui::TreePop();//关闭节点
            }
        }

        if (entity.HasComponent<SpriteRendererComponent>())//如果实体有精灵渲染器组件
        {
            if (ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Sprite Renderer"))//如果节点打开
            {
                auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();//获取精灵渲染器组件
                ImGui::ColorEdit4("Color", glm::value_ptr(spriteRendererComponent.Color));//颜色编辑器
                ImGui::TreePop();//关闭节点
            }
        }
    }

}
