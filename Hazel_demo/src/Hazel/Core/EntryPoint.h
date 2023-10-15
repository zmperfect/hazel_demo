#pragma once
#include "Hazel/Core/Base.h"

#ifdef HZ_PLATFORM_WINDOWS

extern Hazel::Application* Hazel::CreateApplication();//this is defined in the client

int main(int argc, char** argv)
{
	Hazel::Log::Init();//��ʼ����־ϵͳ

	HZ_PROFILE_BEGIN_SESSION("Startup", "HazelProfile-Startup.json");//��ʼ��¼profile
	auto app = Hazel::CreateApplication();//����һ��Ӧ��
	HZ_PROFILE_END_SESSION();//������¼profile

	HZ_PROFILE_BEGIN_SESSION("Runtime", "HazelProfile-Runtime.json");//��ʼ��¼profile
	app->Run();//����Ӧ��
	HZ_PROFILE_END_SESSION();//������¼profile

	HZ_PROFILE_BEGIN_SESSION("Shutdown", "HazelProfile-Shutdown.json");//��ʼ��¼profile
	delete app;//ɾ��Ӧ��
	HZ_PROFILE_END_SESSION();//������¼profile
}

#endif // HZ_PLATFORM_WINDOWS
