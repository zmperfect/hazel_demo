#pragma once

#include <memory>

#include "Hazel/Core/PlatformDetection.h"

#ifdef HZ_DEBUG//�ж��Ƿ�Ϊdebugģʽ
    #if defined(HZ_PLATFORM_WINDOWS)//Windowsƽ̨
        #define HZ_DEBUGBREAK() __debugbreak()//�жϳ���
    #elif defined(HZ_PLATFORM_LINUX)//Linuxƽ̨
        #include <signal.h>
        #define HZ_DEBUGBREAK() raise(SIGTRAP)//�жϳ���
    #else//δ֪ƽ̨
        #error "Platform doesn't support debugbreak yet!"
    #endif
	#define HZ_ENABLE_ASSERTS//���ö���
#else//��Debugģʽ
    #define HZ_DEBUGBREAK()//���жϳ���
#endif

//TODO��ʹ��������condition֮�ⲻ�����κβ���
#ifdef HZ_ENABLE_ASSERTS//���ö���
	#define HZ_ASSERT(x, ...) { if(!(x)) { HZ_ERROR("Assertion Failed: {0}", __VA_ARGS__); HZ_DEBUGBREAK(); } }//����ʧ�ܣ����������Ϣ���жϳ���
	#define HZ_CORE_ASSERT(x, ...) { if(!(x)) { HZ_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); HZ_DEBUGBREAK(); } }//����ʧ�ܣ����������Ϣ���жϳ���
#else
	#define HZ_ASSERT(x, ...)//����ʧ�ܣ������������Ϣ�����жϳ���
	#define HZ_CORE_ASSERT(x, ...)//����ʧ�ܣ������������Ϣ�����жϳ���
#endif

#define BIT(x) (1 << x)//λ����

#define HZ_BIND_EVENT_FN(fn) [this](auto&&... args)->decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }//���¼�����

namespace Hazel {

	template <typename T>
	using Scope = std::unique_ptr<T>;//����ָ��
    template<typename T, typename ... Args>
    constexpr Scope<T> CreateScope(Args&& ... args)//��������ָ��
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

	template <typename T>
	using Ref = std::shared_ptr<T>;//����ָ��
    template<typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
}