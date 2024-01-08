#pragma once

#include <string>
#include <optional>

namespace Hazel {

    class FileDialogs//�ļ��Ի���
    {
    public:
        //���ȡ������Щ���ؿ��ַ���
        static std::optional<std::string> OpenFile(const char* filter);//���ļ�
        static std::optional<std::string> SaveFile(const char* filter);//�����ļ�
    };

}