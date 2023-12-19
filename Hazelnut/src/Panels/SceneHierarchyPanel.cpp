#include "SceneHierarchyPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

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

        //空白处右键菜单
        if (ImGui::BeginPopupContextWindow(0, 1, false))
        {
            if (ImGui::MenuItem("Create Empty Entity"))//如果点击了创建空实体
            {
                m_Context->CreateEntity("Empty Entity");//创建空实体
            }

            ImGui::EndPopup();//结束右键菜单
        }

        ImGui::End();

        ImGui::Begin("Properties");//开始属性面板
        if (m_SelectionContext)//如果选择上下文不为空
        {
            DrawComponents(m_SelectionContext);//绘制组件

            if (ImGui::Button("Add Component"))//如果点击了添加组件按钮
            {
                ImGui::OpenPopup("AddComponent");//打开添加组件弹窗
            }

            if (ImGui::BeginPopup("AddComponent"))
            {
                if (ImGui::MenuItem("Camera"))//如果点击了相机
                {
                        m_SelectionContext.AddComponent<CameraComponent>();//添加相机组件
                        ImGui::CloseCurrentPopup();//关闭当前弹窗
                    }

                if (ImGui::MenuItem("Sprite Renderer"))//如果点击了精灵渲染器
                {
                        m_SelectionContext.AddComponent<SpriteRendererComponent>();//添加精灵渲染器组件
                        ImGui::CloseCurrentPopup();//关闭当前弹窗
                    }

                    ImGui::EndPopup();//结束添加组件弹窗
            }
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

        bool entityDeleted = false;//实体是否被删除
        if (ImGui::BeginPopupContextItem())//如果右键点击了节点
        {
            if (ImGui::MenuItem("Delete Entity"))//如果点击了删除实体
            {
                entityDeleted = true;//实体被删除
            }

            ImGui::EndPopup();//结束右键菜单
        }

        if (opened)//如果节点打开
        {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;//设置节点标志
            bool opened = ImGui::TreeNodeEx((void*)10000, flags, tag.c_str());//绘制节点
            if (opened)//如果节点打开
                ImGui::TreePop();//关闭节点
            ImGui::TreePop();//关闭节点
        }

        if (entityDeleted)//如果实体被删除
        {
            m_Context->DestroyEntity(entity);//销毁实体
            if (m_SelectionContext == entity)//如果选择上下文是实体
                m_SelectionContext = {};//清空选择上下文
        }

    }

    static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
    {
        ImGui::PushID(label.c_str());//压入ID

        ImGui::Columns(2);//设置列数
        ImGui::SetColumnWidth(0, columnWidth);//设置列宽
        ImGui::Text(label.c_str());//文本
        ImGui::NextColumn();//下一列

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());//压入多个宽度
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });//压入样式变量

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;//行高
        ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };//按钮大小

        //X轴样式
        ////压入按钮样式
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        if (ImGui::Button("X", buttonSize))
            values.x = resetValue;
        ImGui::PopStyleColor(3);//弹出样式

        ImGui::SameLine();//同一行
        ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");//拖拽浮点数
        ImGui::PopItemWidth();//弹出宽度
        ImGui::SameLine();//同一行

        //Y轴样式
        ////压入按钮样式
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        if (ImGui::Button("Y", buttonSize))
            values.x = resetValue;
        ImGui::PopStyleColor(3);//弹出样式

        ImGui::SameLine();//同一行
        ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");//拖拽浮点数
        ImGui::PopItemWidth();//弹出宽度
        ImGui::SameLine();//同一行

        //Z轴样式
        ////压入按钮样式
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        if (ImGui::Button("Z", buttonSize))
            values.x = resetValue;
        ImGui::PopStyleColor(3);//弹出样式

        ImGui::SameLine();//同一行
        ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");//拖拽浮点数
        ImGui::PopItemWidth();//弹出宽度
        ImGui::SameLine();//同一行

        ImGui::PopStyleVar();//弹出样式变量

        ImGui::Columns(1);//设置列数

        ImGui::PopID();//弹出ID
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

        const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;//节点标志

        if (entity.HasComponent<TransformComponent>())
        {
            bool open = ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), treeNodeFlags, "Transform");//节点

            if (open)//如果节点打开
            {
                auto& tc = entity.GetComponent<TransformComponent>();//获取变换组件
                DrawVec3Control("Translation", tc.Translation);//绘制向量控件
                glm::vec3 rotation = glm::degrees(tc.Rotation);//旋转转换为角度
                DrawVec3Control("Rotation", rotation);//绘制向量控件
                tc.Rotation = glm::radians(rotation);//角度转换为旋转
                DrawVec3Control("Scale", tc.Scale, 1.0f);//绘制向量控件

                ImGui::TreePop();//关闭节点
            }
        }

        if (entity.HasComponent<CameraComponent>())//如果实体有相机组件
        {
            if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), treeNodeFlags, "Camera"))//如果节点打开
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
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
            bool open = ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(), treeNodeFlags, "Sprite Renderer");//节点
            ImGui::SameLine(ImGui::GetWindowWidth() - 25.0f);
            if (ImGui::Button("+", ImVec2{ 20, 20 }))//按钮
            {
                ImGui::OpenPopup("ComponentSettings");//打开弹出窗口
            }
            ImGui::PopStyleVar();//弹出样式变量

            bool removeComponent = false;//是否移除组件
            if (ImGui::BeginPopup("ComponentSettings"))//如果弹出窗口打开
            {
                if (ImGui::MenuItem("Remove component"))//菜单项
                {
                    removeComponent = true;//移除组件
                }

                ImGui::EndPopup();//关闭弹出窗口
            }

            if (open)//如果节点打开
            {
                auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();//获取精灵渲染器组件
                ImGui::ColorEdit4("Color", glm::value_ptr(spriteRendererComponent.Color));//颜色编辑器
                ImGui::TreePop();//关闭节点
            }

            if (removeComponent)//如果移除组件
            {
                entity.RemoveComponent<SpriteRendererComponent>();//移除精灵渲染器组件
            }
        }
    }

}
