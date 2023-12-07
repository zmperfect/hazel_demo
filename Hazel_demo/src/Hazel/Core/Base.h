#pragma once

#include <memory>

#include "Hazel/Core/PlatformDetection.h"

#ifdef HZ_DEBUG//判断是否为debug模式
    #if defined(HZ_PLATFORM_WINDOWS)//Windows平台
        #define HZ_DEBUGBREAK() __debugbreak()//中断程序
    #elif defined(HZ_PLATFORM_LINUX)//Linux平台
        #include <signal.h>
        #define HZ_DEBUGBREAK() raise(SIGTRAP)//中断程序
    #else//未知平台
        #error "Platform doesn't support debugbreak yet!"
    #endif
	#define HZ_ENABLE_ASSERTS//启用断言
#else//非Debug模式
    #define HZ_DEBUGBREAK()//不中断程序
#endif

#define HZ_EXPAND_MACRO(x) x//展开宏
#define HZ_STRINGIFY_MACRO(x) #x//将宏转换为字符串

#define BIT(x) (1 << x)//位运算

#define HZ_BIND_EVENT_FN(fn) [this](auto&&... args)->decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }//绑定事件函数

namespace Hazel {

	template <typename T>
	using Scope = std::unique_ptr<T>;//智能指针
    template<typename T, typename ... Args>
    constexpr Scope<T> CreateScope(Args&& ... args)//创建智能指针
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

	template <typename T>
	using Ref = std::shared_ptr<T>;//智能指针
    template<typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
}

#include "Hazel/Core/Log.h"
#include "Hazel/Core/Assert.h"