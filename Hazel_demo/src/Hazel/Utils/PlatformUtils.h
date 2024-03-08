#pragma once

#include <string>

namespace Hazel {

    class FileDialogs//文件对话框
    {
    public:
        //如果取消，这些返回空字符串
        static std::string OpenFile(const char* filter);//打开文件
        static std::string SaveFile(const char* filter);//保存文件
    };

}