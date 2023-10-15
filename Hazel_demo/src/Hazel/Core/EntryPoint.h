#pragma once
#include "Hazel/Core/Base.h"

#ifdef HZ_PLATFORM_WINDOWS

extern Hazel::Application* Hazel::CreateApplication();//this is defined in the client

int main(int argc, char** argv)
{
	Hazel::Log::Init();//初始化日志系统

	HZ_PROFILE_BEGIN_SESSION("Startup", "HazelProfile-Startup.json");//开始记录profile
	auto app = Hazel::CreateApplication();//创建一个应用
	HZ_PROFILE_END_SESSION();//结束记录profile

	HZ_PROFILE_BEGIN_SESSION("Runtime", "HazelProfile-Runtime.json");//开始记录profile
	app->Run();//运行应用
	HZ_PROFILE_END_SESSION();//结束记录profile

	HZ_PROFILE_BEGIN_SESSION("Shutdown", "HazelProfile-Shutdown.json");//开始记录profile
	delete app;//删除应用
	HZ_PROFILE_END_SESSION();//结束记录profile
}

#endif // HZ_PLATFORM_WINDOWS
