#pragma once

//使用预定义宏的平台检测
#ifdef _WIN32 //Windows平台
    #ifdef _WIN64 //64位
        #define HZ_PLATFORM_WINDOWS
    #else//32位
        #error "x86 Builds are not supported!"
    #endif
#elif defined(__APPLE__) || defined(__MACH__)//苹果平台
    #include <TargetConditionals.h>
    /* TARGET_OS_MAC存在于苹果平台的所有系统中
     * 所以必须检查所有的系统来确保运行在MAC上*/
    #if TARGET_IPHONE_SIMULATOR == 1//模拟器
        #error "IOS simulator is not supported!"
    #elif TARGET_OS_IPHONE == 1//IOS
        #define HZ_PLATFORM_IOS
        #error "IOS is not supported!"
    #elif TARGET_OS_MAC == 1//MAC
        #define HZ_PLATFORM_MACOS
        #error "MacOS is not supported!"
    #else//未知
        #error "Unknown Apple platform!"
#endif

//在Linux平台上编译前必须检查是否是安卓平台
#elif defined(__ANDROID__)//安卓平台
    #define HZ_PLATFORM_ANDROID
    #error "Android is not supported!"
#elif defined(__linux__)//Linux平台
    #define HZ_PLATFORM_LINUX
    #error "Linux is not supported!"
#else//未知平台
    #error "Unknown platform!"
#endif // 平台检测结束