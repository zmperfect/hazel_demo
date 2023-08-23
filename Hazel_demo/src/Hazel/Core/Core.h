#pragma once

#include <memory>

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

//DLLsupport
#ifdef HZ_PLATFORM_WINDOWS//Windows平台
    #if HZ_DYNAMIC_LINK//动态链接
        #ifdef HZ_BUILD_DLL//编译DLL
            #define HAZEL_API __declspec(dllexport)//导出
        #else//未编译DLL
            #define HAZEL_API __declspec(dllimport)//导入
        #endif
    #else//静态链接
        #define HAZEL_API
    #endif
#else//非Windows平台
    #error Hazel only supports Windows!
#endif //End of DLL support

#ifdef HZ_DEBUG//判断是否为debug模式
	#define HZ_ENABLE_ASSERTS//启用断言
#endif

#ifdef HZ_ENABLE_ASSERTS//启用断言
	#define HZ_ASSERT(x, ...) { if(!(x)) { HZ_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }//断言失败，输出错误信息，中断程序
	#define HZ_CORE_ASSERT(x, ...) { if(!(x)) { HZ_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }//断言失败，输出错误信息，中断程序
#else
	#define HZ_ASSERT(x, ...)//断言失败，不输出错误信息，不中断程序
	#define HZ_CORE_ASSERT(x, ...)//断言失败，不输出错误信息，不中断程序
#endif

#define BIT(x) (1 << x)//位运算

#define HZ_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)//绑定事件函数

namespace Hazel {

	template <typename T>
	using Scope = std::unique_ptr<T>;//智能指针

	template <typename T>
	using Ref = std::shared_ptr<T>;//智能指针
}