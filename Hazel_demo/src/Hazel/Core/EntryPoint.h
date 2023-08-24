#pragma once

#ifdef HZ_PLATFORM_WINDOWS

extern Hazel::Application* Hazel::CreateApplication();//this is defined in the client

int main(int argc, char** argv)
{
	Hazel::Log::Init();//初始化日志系统
	HZ_CORE_WARN("Initialized Log!");//输出日志
	int a = 5;//测试输出变量
	HZ_INFO("Hello! Var={0}", a);//输出变量


	auto app = Hazel::CreateApplication();//创建一个应用
	app->Run();//运行应用
	delete app;//删除应用
}

#endif // HZ_PLATFORM_WINDOWS
