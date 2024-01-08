#include "hzpch.h"
#include "Hazel/Utils/PlatformUtils.h"

#include <sstream>
#include <commdlg.h> //Common Dialog APIs
#include <GLFW/glfw3.h>

//使用glfwGetWin32Window函数获取窗口句柄必须包含下面两行代码
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "Hazel/Core/Application.h"

namespace Hazel {

    std::optional<std::string> FileDialogs::OpenFile(const char* filter)
    {
        OPENFILENAMEA ofn;//打开文件名，A表示ANSI字符集
        CHAR szFile[260] = { 0 };//文件名
        CHAR currentDir[256] = { 0 };//当前目录

        //初始化OPENFILENAME结构体
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);//结构的长度，用户填写
        ofn.hwndOwner = glfwGetWin32Window(static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow()));//指向所有者对话框窗口的句柄。这个成员可以是任意有效窗口句柄，或如果对话框没有所有者它可以为NULL。
        ofn.lpstrFile = szFile;//指向一个包含文件名的缓冲区。如果这个缓冲区中已经包含了一个文件名，那么对话框初始化的时候将显示这个文件名。当用户选择了一个文件的时候，函数在这里返回新的文件名。如果不需要初始值，这个缓冲的第一个字符必须是NULL。当GetOpenFileName或GetSaveFileName函数返回成功时，这个缓冲包含驱动器，路径，文件名，及所选择的文件的扩展名。
        ofn.nMaxFile = sizeof(szFile);//指定lpstrFile缓冲区的长度，以字符为单位。这个长度必须大于MAX_PATH+1。MAX_PATH是一个常数，定义在Windows.h中，它的值是260。

        if (GetCurrentDirectoryA(256, currentDir) == 0)//获取当前目录
        {
            return std::nullopt;//返回空
        }

        ofn.lpstrFilter = filter; //指定文件名筛选字符串，该字段决定了对话框中“文件类型”下拉式列表框中的内容，字符串可以由多组内容组成，每组包括一个说明字符串和一个筛选字符串，字符串的最后用两个0结束（即两个NULL字符）。如下面的字符串将在列表框中显示两项内容，选择不同项目的时候分别列出“* .txt”文件或者所有文件“* .*" ：
        //'Text Files(*.txt)', 0, '*.txt', 0, 'All Files(*.*)', 0, '*.*', 0, 0   筛选字符串中也可以同时指定多个扩展名，中间用分号隔开，如'*.txt;*.doc'。
        ofn.nFilterIndex = 1;//指定缺省的文件扩展名索引，即列表框中的默认选择项。如果nFilterIndex为1，那么列表框中的第一项就是缺省选择项。如果nFilterIndex为2，那么列表框中的第二项就是缺省选择项，以此类推。如果nFilterIndex为0，那么列表框中将没有缺省选择项。
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;//指定对话框的一些属性，如是否包含“读取”按钮，是否包含“写入”按钮，是否允许多选等等。具体的属性请参考MSDN中关于OPENFILENAME结构体的说明。

        if (GetOpenFileNameA(&ofn) == TRUE)//打开文件对话框
        {
            return ofn.lpstrFile;//返回文件名
        }
        return std::nullopt;//返回空

    }

    std::optional<std::string> FileDialogs::SaveFile(const char* filter)
    {
        OPENFILENAMEA ofn;//打开文件名，A表示ANSI字符集
        CHAR szFile[260] = { 0 };//文件名
        CHAR currentDir[256] = { 0 };//当前目录

        //初始化OPENFILENAME结构体
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);//结构的长度，用户填写
        ofn.hwndOwner = glfwGetWin32Window(static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow()));
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        if (GetCurrentDirectoryA(256, currentDir) == 0)
        {
            return std::nullopt;//返回空
        }
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;//指定对话框的一些属性，如是否包含“读取”按钮，是否包含“写入”按钮，是否允许多选等等。具体的属性请参考MSDN中关于OPENFILENAME结构体的说明。

        // 设置默认文件名
        ofn.lpstrDefExt  = strchr(filter, '\0') + 1;//指向一个以NULL结尾的字符串，用于指定默认的扩展名。如果用户指定的文件名没有扩展名，那么缺省的扩展名就会被加到文件名后面。如果用户指定的文件名已经有扩展名，那么这个缺省扩展名就不会被加到文件名后面。如果这个成员为NULL，那么缺省扩展名将不会被加到文件名后面。

        if (GetSaveFileNameA(&ofn) == TRUE)
        {
            return ofn.lpstrFile;//返回文件名
        }
        return std::nullopt;//返回空
    }
}