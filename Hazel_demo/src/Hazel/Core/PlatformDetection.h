#pragma once

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