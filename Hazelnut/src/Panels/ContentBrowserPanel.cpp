#include "hzpch.h"
#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>

namespace Hazel {

    // 一旦有了项目，就改变这个路径
    static const std::filesystem::path s_AssetPath = "assets";

    ContentBrowserPanel::ContentBrowserPanel()
        : m_CurrentDirectory(s_AssetPath)
    {
        m_DirectoryIcon = Texture2D::Create("Resources/Icons/ContentBrowser/DirectoryIcon.png");
        m_FileIcon = Texture2D::Create("Resources/Icons/ContentBrowser/FileIcon.png");
    }

    // 在 ImGui 中渲染内容浏览器面板
    void ContentBrowserPanel::OnImGuiRender()
    {
        ImGui::Begin("Content Browser"); // 开始一个新的 ImGui 窗口，标题为 "Content Browser"

        // 如果当前目录不是 assets 目录，显示"<-"按钮，点击后将当前目录设置为其父目录，实现返回上一级目录的功能
        if (m_CurrentDirectory != std::filesystem::path(s_AssetPath))
        {
            if (ImGui::Button("<-"))
            {
                    m_CurrentDirectory = m_CurrentDirectory.parent_path();
            }
        }

        // 设置缩略图的格式
        static float padding = 16.0f;// 设置缩略图之间的间距
        static float thumbnailSize = 128.0f;// 设置缩略图的大小
        float cellSize = thumbnailSize + padding;// 设置每个单元格的大小

        float panelWidth = ImGui::GetContentRegionAvail().x;// 获取内容区域的宽度
        int columnCount = (int)(panelWidth / cellSize);// 计算每行可以容纳多少个缩略图
        if (columnCount < 1)
            columnCount = 1;

        ImGui::Columns(columnCount, 0, false);// 设置列数

        // 使用 `std::filesystem::directory_iterator` 遍历当前目录下的所有条目（文件和子目录）。对于每个条目，显示一个按钮，按钮的文本是条目的相对路径。如果条目是一个目录，点击按钮后将当前目录设置为这个子目录，实现进入子目录的功能。如果条目是一个文件，这里没有实现任何功能，但你可以在这里添加打开文件或其他操作的代码。
        for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
        {
            const auto& path = directoryEntry.path();
            auto relativePath = std::filesystem::relative(path, s_AssetPath);
            std::string filenameString = relativePath.filename().string();

            Ref<Texture2D> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;//设置图标
            ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });//设置按钮
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                if (directoryEntry.is_directory())
                    m_CurrentDirectory /= path.filename();
            }
            ImGui::TextWrapped(filenameString.c_str());//设置文本

            ImGui::NextColumn();
        }

        ImGui::Columns(1);// 设置列数为1，结束列布局

        ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);// 设置缩略图大小的滑块
        ImGui::SliderFloat("Padding", &padding, 0, 32);// 设置缩略图之间的间距的滑块

        // TODO: 状态栏
        ImGui::End();
    }
    
}