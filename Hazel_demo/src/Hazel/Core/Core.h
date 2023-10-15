#pragma once

#include <memory>

//ʹ��Ԥ������ƽ̨���
#ifdef _WIN32 //Windowsƽ̨
    #ifdef _WIN64 //64λ
        #define HZ_PLATFORM_WINDOWS
    #else//32λ
        #error "x86 Builds are not supported!"
    #endif
#elif defined(__APPLE__) || defined(__MACH__)//ƻ��ƽ̨
	#include <TargetConditionals.h>
    /* TARGET_OS_MAC������ƻ��ƽ̨������ϵͳ��
     * ���Ա��������е�ϵͳ��ȷ��������MAC��*/
    #if TARGET_IPHONE_SIMULATOR == 1//ģ����
        #error "IOS simulator is not supported!"
    #elif TARGET_OS_IPHONE == 1//IOS
        #define HZ_PLATFORM_IOS
        #error "IOS is not supported!"
    #elif TARGET_OS_MAC == 1//MAC
        #define HZ_PLATFORM_MACOS
        #error "MacOS is not supported!"
    #else//δ֪
        #error "Unknown Apple platform!"
    #endif

//��Linuxƽ̨�ϱ���ǰ�������Ƿ��ǰ�׿ƽ̨
#elif defined(__ANDROID__)//��׿ƽ̨
    #define HZ_PLATFORM_ANDROID
    #error "Android is not supported!"
#elif defined(__linux__)//Linuxƽ̨
    #define HZ_PLATFORM_LINUX
    #error "Linux is not supported!"
#else//δ֪ƽ̨
    #error "Unknown platform!"
#endif // ƽ̨������

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

#ifdef HZ_ENABLE_ASSERTS//���ö���
	#define HZ_ASSERT(x, ...) { if(!(x)) { HZ_ERROR("Assertion Failed: {0}", __VA_ARGS__); HZ_DEBUGBREAK(); } }//����ʧ�ܣ����������Ϣ���жϳ���
	#define HZ_CORE_ASSERT(x, ...) { if(!(x)) { HZ_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); HZ_DEBUGBREAK(); } }//����ʧ�ܣ����������Ϣ���жϳ���
#else
	#define HZ_ASSERT(x, ...)//����ʧ�ܣ������������Ϣ�����жϳ���
	#define HZ_CORE_ASSERT(x, ...)//����ʧ�ܣ������������Ϣ�����жϳ���
#endif

#define BIT(x) (1 << x)//λ����

#define HZ_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)//���¼�����

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