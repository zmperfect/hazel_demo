#pragma once

#include <memory>

#ifdef HZ_PLATFORM_WINDOWS//�ж�ƽ̨
#if HZ_DYNAMIC_LINK//�ж��Ƿ�Ϊ��̬���ӿ�
	#ifdef HZ_BUILD_DLL//�Ƿ񹹽�dll
		#define HAZEL_API _declspec(dllexport)
	#else
		#define HAZEL_API _declspec(dllimport)
	#endif
#else
	#define HAZEL_API//��̬���ӿ�
#endif
#else
	//������ʾ
	#error Hazel only support Windows!
#endif

#ifdef HZ_DEBUG//�ж��Ƿ�Ϊdebugģʽ
	#define HZ_ENABLE_ASSERTS//���ö���
#endif

#ifdef HZ_ENABLE_ASSERTS//���ö���
	#define HZ_ASSERT(x, ...) { if(!(x)) { HZ_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }//����ʧ�ܣ����������Ϣ���жϳ���
	#define HZ_CORE_ASSERT(x, ...) { if(!(x)) { HZ_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }//����ʧ�ܣ����������Ϣ���жϳ���
#else
	#define HZ_ASSERT(x, ...)//����ʧ�ܣ������������Ϣ�����жϳ���
	#define HZ_CORE_ASSERT(x, ...)//����ʧ�ܣ������������Ϣ�����жϳ���
#endif

#define BIT(x) (1 << x)//λ����

#define HZ_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)//���¼�����

namespace Hazel {

	template <typename T>
	using Scope = std::unique_ptr<T>;//����ָ��

	template <typename T>
	using Ref = std::shared_ptr<T>;//����ָ��
}