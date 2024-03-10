#include "SceneHierarchyPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

#include "Hazel/Scene/Components.h"
#include <cstring>

//Microsoft C++编译器不支持C++标准库，需要一下的定义来忽略关于std::strncpy()的警告

#ifdef _MSVC_LANG   //如果是Microsoft C++编译器
    #define _CRT_SECURE_NO_WARNINGS//忽略关于std::strncpy()的警告
#endif

namespace Hazel {

    extern const std::filesystem::path g_AssetPath;

    SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene)
    {
        SetContext(scene);//设置上下文
    }

    void SceneHierarchyPanel::SetContext(const Ref<Scene>& scene)
    {
        m_Context = scene;//设置上下文
        m_SelectionContext = {};//清空选择上下文
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
        }

        ImGui::End();//结束场景层次面板
    }

    void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
    {
        m_SelectionContext = entity;//设置选择上下文
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
        ImGuiIO& io = ImGui::GetIO();//获取IO
        auto boldFont = io.Fonts->Fonts[0];//获取粗体字体

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
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });//压入按钮样式
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });//压入按钮悬停样式
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });//压入按钮激活样式
        ImGui::PushFont(boldFont);//压入粗体字体
        if (ImGui::Button("X", buttonSize))
            values.x = resetValue;
        ImGui::PopFont();//弹出字体
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
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Y", buttonSize))
            values.x = resetValue;
        ImGui::PopFont();
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

    template<typename T, typename UIFunction>
    static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)//绘制组件
    {
        const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;//节点标志,默认打开,带框架,允许重叠,带填充

        if (entity.HasComponent<T>())//如果实体有组件
        {
            auto& component = entity.GetComponent<T>();//获取组件
            ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();//获取内容区域可用大小

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });//压入样式变量
            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;//行高
            ImGui::Separator();//分割线
            bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());//节点
            ImGui::PopStyleVar();//弹出样式变量
            ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);//同一行
            if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))//按钮
            {
                ImGui::OpenPopup("ComponentSettings");//打开弹出窗口
            }

            bool removeComponent = false;//是否移除组件
            if (ImGui::BeginPopup("ComponentSettings"))//如果弹出窗口打开
            {
                if (ImGui::MenuItem("Remove Component"))//如果点击了移除组件
                    entity.RemoveComponent<T>();//移除组件

                ImGui::EndPopup();//结束弹出窗口
            }

            if (open)//如果节点打开
            {
                uiFunction(component);//UI函数
                ImGui::TreePop();//关闭节点
            }

            if (removeComponent)//如果移除组件
                entity.RemoveComponent<T>();//移除组件
        }
    }

    void SceneHierarchyPanel::DrawComponents(Entity entity)
    {
        if (entity.HasComponent<TagComponent>())//如果实体有标签组件
        {
            auto& tag = entity.GetComponent<TagComponent>().Tag;//获取标签组件

            char buffer[256];//缓冲区
            memset(buffer, 0, sizeof(buffer));//清空缓冲区
            std::strncpy(buffer, tag.c_str(), sizeof(buffer));//复制标签到缓冲区
            if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))//如果输入标签，##Tag表示不显示标签
            {
                tag = std::string(buffer);//设置标签
            }
        }

        ImGui::SameLine();//同一行
        ImGui::PushItemWidth(-1);//压入宽度

        if (ImGui::Button("Add Component"))//如果点击了添加组件按钮
        {
            ImGui::OpenPopup("AddComponent");//打开弹出窗口
        }

        if (ImGui::BeginPopup("AddComponent"))//如果弹出窗口打开
        {
            if (ImGui::MenuItem("Camera"))//如果点击了相机
            {
                if (!m_SelectionContext.HasComponent<CameraComponent>())//如果选择的实体没有相机组件
                    m_SelectionContext.AddComponent<CameraComponent>();//添加相机组件
                else
                    HZ_CORE_WARN("This entity already has a camera component!");
                ImGui::CloseCurrentPopup();//关闭当前弹出窗口
            }

            if (ImGui::MenuItem("Sprite Renderer"))//如果点击了精灵渲染器
            {
                if (!m_SelectionContext.HasComponent<SpriteRendererComponent>())//如果选择的实体没有精灵渲染器组件
                    m_SelectionContext.AddComponent<SpriteRendererComponent>();//添加精灵渲染器组件
                else
                    HZ_CORE_WARN("This entity already has a sprite renderer component!");
                ImGui::CloseCurrentPopup();//关闭当前弹出窗口
            }

            ImGui::EndPopup();//结束弹出窗口
        }

        ImGui::PopItemWidth();//弹出宽度

        DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
        {
            DrawVec3Control("Translation", component.Translation);//绘制变换控件
            glm::vec3 rotation = glm::degrees(component.Rotation);//旋转角度
            DrawVec3Control("Rotation", rotation);//绘制旋转控件
            component.Rotation = glm::radians(rotation);//旋转赋值
            DrawVec3Control("Scale", component.Scale, 1.0f);//绘制缩放控件
        });//绘制变换组件
        
        DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
        {
            auto& camera = component.Camera;//获取相机组件

            ImGui::Checkbox("Primary", &component.Primary);//主相机复选框
            const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };//投影类型字符串(透视，正交)
            const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];//当前投影类型字符串
            if (ImGui::BeginCombo("Projection", currentProjectionTypeString))//如果投影类型下拉框打开
            {
                for (int i = 0; i < 2; i++)//遍历投影类型
                {
                    bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];//是否选中
                    if (ImGui::Selectable(projectionTypeStrings[i], isSelected))//如果选择了投影类型
                    {
                        currentProjectionTypeString = projectionTypeStrings[i];//设置当前投影类型
                        camera.SetProjectionType((SceneCamera::ProjectionType)i);//设置投影类型
                    }

                    if (isSelected)//如果选中
                        ImGui::SetItemDefaultFocus();//设置默认焦点
                }

                ImGui::EndCombo();//结束投影类型下拉框
            }

            if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)//如果是透视投影
            {
                float perspectiveVerticalFOV = glm::degrees(camera.GetPerspectiveVerticalFOV());//垂直视角
                if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFOV))//拖拽浮点数
                    camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFOV));//设置垂直视角

                float perspectiveNear = camera.GetPerspectiveNearClip();//近裁剪面
                if (ImGui::DragFloat("Near", &perspectiveNear))//拖拽浮点数
                    camera.SetPerspectiveNearClip(perspectiveNear);//设置近裁剪面

                float perspectiveFar = camera.GetPerspectiveFarClip();//远裁剪面
                if (ImGui::DragFloat("Far", &perspectiveFar))//拖拽浮点数
                    camera.SetPerspectiveFarClip(perspectiveFar);//设置远裁剪面
            }

            if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)//如果是正交投影
            {
                float orthographicSize = camera.GetOrthographicSize();//正交大小
                if (ImGui::DragFloat("Size", &orthographicSize))//拖拽浮点数
                    camera.SetOrthographicSize(orthographicSize);//设置正交大小

                float orthographicNear = camera.GetOrthographicNearClip();//近裁剪面
                if (ImGui::DragFloat("Near", &orthographicNear))//拖拽浮点数
                    camera.SetOrthographicNearClip(orthographicNear);//设置近裁剪面

                float orthographicFar = camera.GetOrthographicFarClip();//远裁剪面
                if (ImGui::DragFloat("Far", &orthographicFar))//拖拽浮点数
                    camera.SetOrthographicFarClip(orthographicFar);//设置远裁剪面

                ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);//固定宽高比复选框
            }
        });//绘制相机组件

        DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
        {
            ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));//颜色编辑器

            // Texture的拖拽放置
            ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
					Ref<Texture2D> texture = Texture2D::Create(texturePath.string());
                    if (texture->IsLoaded())
                        component.Texture = texture;//设置纹理
                    else
                        HZ_CORE_WARN("Failed to load texture '{0}'", texturePath.string());
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f);
        });//绘制精灵渲染器组件
    }

}
