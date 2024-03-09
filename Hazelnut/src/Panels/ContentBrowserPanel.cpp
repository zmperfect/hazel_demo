#include "hzpch.h"
#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>

namespace Hazel {

    // 一旦有了项目，就改变这个路径
    static const std::filesystem::path s_AssetPath = "assets";

    ContentBrowserPanel::ContentBrowserPanel()
        : m_CurrentDirectory(s_AssetPath)
    {
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

        // 使用 `std::filesystem::directory_iterator` 遍历当前目录下的所有条目（文件和子目录）。对于每个条目，显示一个按钮，按钮的文本是条目的相对路径。如果条目是一个目录，点击按钮后将当前目录设置为这个子目录，实现进入子目录的功能。如果条目是一个文件，这里没有实现任何功能，但你可以在这里添加打开文件或其他操作的代码。
        for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
        {
            const auto& path = directoryEntry.path();
            auto relativePath = std::filesystem::relative(path, s_AssetPath);
            if (directoryEntry.is_directory())
            {
                if (ImGui::Button(relativePath.string().c_str()))
                    m_CurrentDirectory /= path.filename();
            }
            else
            {
                if (ImGui::Button(relativePath.string().c_str()))
                {
                }
            }
        }

        ImGui::End();
    }
    
}