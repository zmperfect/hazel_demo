#pragma once

#ifdef HZ_PLATFORM_WINDOWS

extern Hazel::Application* Hazel::CreateApplication();//this is defined in the client

int main(int argc, char** argv)
{
	Hazel::Log::Init();//��ʼ����־ϵͳ
	HZ_CORE_WARN("Initialized Log!");//�����־
	int a = 5;//�����������
	HZ_INFO("Hello! Var={0}", a);//�������


	auto app = Hazel::CreateApplication();//����һ��Ӧ��
	app->Run();//����Ӧ��
	delete app;//ɾ��Ӧ��
}

#endif // HZ_PLATFORM_WINDOWS
