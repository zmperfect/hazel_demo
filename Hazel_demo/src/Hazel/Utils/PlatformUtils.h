#pragma once

#include <string>

namespace Hazel {

    class FileDialogs//�ļ��Ի���
    {
    public:
        //���ȡ������Щ���ؿ��ַ���
        static std::string OpenFile(const char* filter);//���ļ�
        static std::string SaveFile(const char* filter);//�����ļ�
    };

}