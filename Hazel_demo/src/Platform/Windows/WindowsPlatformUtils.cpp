#include "hzpch.h"
#include "Hazel/Utils/PlatformUtils.h"

#include <sstream>
#include <commdlg.h> //Common Dialog APIs
#include <GLFW/glfw3.h>

//ʹ��glfwGetWin32Window������ȡ���ھ����������������д���
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "Hazel/Core/Application.h"

namespace Hazel {

    std::optional<std::string> FileDialogs::OpenFile(const char* filter)
    {
        OPENFILENAMEA ofn;//���ļ�����A��ʾANSI�ַ���
        CHAR szFile[260] = { 0 };//�ļ���
        CHAR currentDir[256] = { 0 };//��ǰĿ¼

        //��ʼ��OPENFILENAME�ṹ��
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);//�ṹ�ĳ��ȣ��û���д
        ofn.hwndOwner = glfwGetWin32Window(static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow()));//ָ�������߶Ի��򴰿ڵľ���������Ա������������Ч���ھ����������Ի���û��������������ΪNULL��
        ofn.lpstrFile = szFile;//ָ��һ�������ļ����Ļ����������������������Ѿ�������һ���ļ�������ô�Ի����ʼ����ʱ����ʾ����ļ��������û�ѡ����һ���ļ���ʱ�򣬺��������ﷵ���µ��ļ������������Ҫ��ʼֵ���������ĵ�һ���ַ�������NULL����GetOpenFileName��GetSaveFileName�������سɹ�ʱ��������������������·�����ļ���������ѡ����ļ�����չ����
        ofn.nMaxFile = sizeof(szFile);//ָ��lpstrFile�������ĳ��ȣ����ַ�Ϊ��λ��������ȱ������MAX_PATH+1��MAX_PATH��һ��������������Windows.h�У�����ֵ��260��

        if (GetCurrentDirectoryA(256, currentDir) == 0)//��ȡ��ǰĿ¼
        {
            return std::nullopt;//���ؿ�
        }

        ofn.lpstrFilter = filter; //ָ���ļ���ɸѡ�ַ��������ֶξ����˶Ի����С��ļ����͡�����ʽ�б���е����ݣ��ַ��������ɶ���������ɣ�ÿ�����һ��˵���ַ�����һ��ɸѡ�ַ������ַ��������������0������������NULL�ַ�������������ַ��������б������ʾ�������ݣ�ѡ��ͬ��Ŀ��ʱ��ֱ��г���* .txt���ļ����������ļ���* .*" ��
        //'Text Files(*.txt)', 0, '*.txt', 0, 'All Files(*.*)', 0, '*.*', 0, 0   ɸѡ�ַ�����Ҳ����ͬʱָ�������չ�����м��÷ֺŸ�������'*.txt;*.doc'��
        ofn.nFilterIndex = 1;//ָ��ȱʡ���ļ���չ�����������б���е�Ĭ��ѡ������nFilterIndexΪ1����ô�б���еĵ�һ�����ȱʡѡ������nFilterIndexΪ2����ô�б���еĵڶ������ȱʡѡ����Դ����ơ����nFilterIndexΪ0����ô�б���н�û��ȱʡѡ���
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;//ָ���Ի����һЩ���ԣ����Ƿ��������ȡ����ť���Ƿ������д�롱��ť���Ƿ������ѡ�ȵȡ������������ο�MSDN�й���OPENFILENAME�ṹ���˵����

        if (GetOpenFileNameA(&ofn) == TRUE)//���ļ��Ի���
        {
            return ofn.lpstrFile;//�����ļ���
        }
        return std::nullopt;//���ؿ�

    }

    std::optional<std::string> FileDialogs::SaveFile(const char* filter)
    {
        OPENFILENAMEA ofn;//���ļ�����A��ʾANSI�ַ���
        CHAR szFile[260] = { 0 };//�ļ���
        CHAR currentDir[256] = { 0 };//��ǰĿ¼

        //��ʼ��OPENFILENAME�ṹ��
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);//�ṹ�ĳ��ȣ��û���д
        ofn.hwndOwner = glfwGetWin32Window(static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow()));
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        if (GetCurrentDirectoryA(256, currentDir) == 0)
        {
            return std::nullopt;//���ؿ�
        }
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;//ָ���Ի����һЩ���ԣ����Ƿ��������ȡ����ť���Ƿ������д�롱��ť���Ƿ������ѡ�ȵȡ������������ο�MSDN�й���OPENFILENAME�ṹ���˵����

        // ����Ĭ���ļ���
        ofn.lpstrDefExt  = strchr(filter, '\0') + 1;//ָ��һ����NULL��β���ַ���������ָ��Ĭ�ϵ���չ��������û�ָ�����ļ���û����չ������ôȱʡ����չ���ͻᱻ�ӵ��ļ������档����û�ָ�����ļ����Ѿ�����չ������ô���ȱʡ��չ���Ͳ��ᱻ�ӵ��ļ������档��������ԱΪNULL����ôȱʡ��չ�������ᱻ�ӵ��ļ������档

        if (GetSaveFileNameA(&ofn) == TRUE)
        {
            return ofn.lpstrFile;//�����ļ���
        }
        return std::nullopt;//���ؿ�
    }
}