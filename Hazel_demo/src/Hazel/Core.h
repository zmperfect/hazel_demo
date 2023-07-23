#pragma once

#include <memory>

#ifdef HZ_PLATFORM_WINDOWS//判断平台
#if HZ_DYNAMIC_LINK//判断是否为动态链接库
	#ifdef HZ_BUILD_DLL//是否构建dll
		#define HAZEL_API _declspec(dllexport)
	#else
		#define HAZEL_API _declspec(dllimport)
	#endif
#else
	#define HAZEL_API//静态链接库
#endif
#else
	//错误提示
	#error Hazel only support Windows!
#endif

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