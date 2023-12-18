#pragma once

#include "Hazel/Core/Base.h"
#include "Hazel/Core/Log.h"

#ifdef HZ_ENABLE_ASSERTS

    namespace Hazel::Assert
    {
        //返回一个简单的文件名而不是完整的路径
        constexpr const char* CurrentFileName(const char* path)
        {
            const char* file = path;
            while (*path)
            {
                if (*path == '/' || *path == '\\')
                    file = ++path;
                else
                    path++;
            }
            return file;
        }
    }

    #define HZ_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { HZ##type##ERROR(msg, __VA_ARGS__); HZ_DEBUGBREAK(); } }
    #define HZ_INTERNAL_ASSERT_WITH_MSG(type, check, ...) HZ_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
    #define HZ_INTERNAL_ASSERT_NO_MSG(type, check) HZ_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", HZ_STRINGIFY_MACRO(check), ::Hazel::Assert::CurrentFileName(__FILE__), __LINE__)

    #define HZ_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
    #define HZ_INTERNAL_ASSERT_GET_MACRO(...) HZ_EXPAND_MACRO( HZ_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, HZ_INTERNAL_ASSERT_WITH_MSG, HZ_INTERNAL_ASSERT_NO_MSG) )

    // Currently accepts at least the condition and one additional parameter (the message) being optional
    #define HZ_ASSERT(...) HZ_EXPAND_MACRO( HZ_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
    #define HZ_CORE_ASSERT(...) HZ_EXPAND_MACRO( HZ_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
        #define HZ_ASSERT(...)
        #define HZ_CORE_ASSERT(...)
#endif