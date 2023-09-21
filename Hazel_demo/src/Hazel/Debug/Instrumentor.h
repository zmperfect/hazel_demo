#pragma once

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>

#include <thread>

namespace Hazel {
    //测试用的类
    struct ProfileResult
    {
        std::string Name;//测试的名字
        long long Start, End;//开始时间和结束时间
        uint32_t ThreadID;//线程ID
    };

    struct InstrumentationSession//会话
    {
        std::string Name;//会话的名字
    };
    
    //测试的类
    class Instrumentor
    {
    private:
        InstrumentationSession* m_CurrentSession;//当前会话
        std::ofstream m_OutputStream;//输出流
        int m_ProfileCount;//测试的数量
    public:
        Instrumentor()
            : m_CurrentSession(nullptr), m_ProfileCount(0)
        {
        }

        void BeginSession(const std::string& name, const std::string& filepath = "results.json")//开始一个会话
        {
            m_OutputStream.open(filepath);//打开输出流
            WriteHeader();//写入头部
            m_CurrentSession = new InstrumentationSession{ name };//创建一个会话
        }

        void EndSession()//结束一个会话
        {
            WriteFooter();//写入尾部
            m_OutputStream.close();//关闭输出流
            delete m_CurrentSession;//删除会话
            m_CurrentSession = nullptr;
            m_ProfileCount = 0;
        }

        void WriteProfile(const ProfileResult& result)//写入测试结果
        {
            if (m_ProfileCount++ > 0)
                m_OutputStream << ",";

            std::string name = result.Name;//测试的名字
            std::replace(name.begin(), name.end(), '"', '\'');//将双引号替换为单引号

            //写入测试结果
            m_OutputStream << "{";
            m_OutputStream << "\"cat\":\"function\",";
            m_OutputStream << "\"dur\":" << (result.End - result.Start) << ',';
            m_OutputStream << "\"name\":\"" << name << "\",";
            m_OutputStream << "\"ph\":\"X\",";
            m_OutputStream << "\"pid\":0,";
            m_OutputStream << "\"tid\":" << result.ThreadID << ",";
            m_OutputStream << "\"ts\":" << result.Start;
            m_OutputStream << "}";
            
            m_OutputStream.flush();//清空缓冲区
        }

        void WriteHeader()//写入头部
        {
            m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
            m_OutputStream.flush();//清空缓冲区
        }

        void WriteFooter()//写入尾部
        {
            m_OutputStream << "]}";
            m_OutputStream.flush();//清空缓冲区
        }

        static Instrumentor& Get()//获取测试类的实例
        {
            static Instrumentor instance;
            return instance;
        }
    };

    class InstrumentationTimer//测试的计时器
    {
    public:
        InstrumentationTimer(const char* name)
            : m_Name(name), m_Stopped(false)
        {
            m_StartTimepoint = std::chrono::high_resolution_clock::now();//获取当前时间
        }

        ~InstrumentationTimer()
        {
            if (!m_Stopped)
                Stop();
        }

        void Stop()
        {
            auto endTimepoint = std::chrono::high_resolution_clock::now();//获取当前时间

            long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();//开始时间转换
            long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();//结束时间转换

            uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());//获取线程ID
            Instrumentor::Get().WriteProfile({ m_Name, start, end, threadID });//写入测试结果

            m_Stopped = true;
        }
    private:
        const char* m_Name;//测试的名字
        std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;//开始时间
        bool m_Stopped;//是否停止
    };
}

#define HZ_PROFILE 1
#if HZ_PROFILE

    // Resolve which function signature macro will be used. Note that this only
    // is resolved when the (pre)compiler starts, so the syntax highlighting
    // could mark the wrong one in your editor!
#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
#define HZ_FUNCSIG __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
#define HZ_FUNCSIG __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__)
#define HZ_FUNCSIG __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
#define HZ_FUNCSIG __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
#define HZ_FUNCSIG __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#define HZ_FUNCSIG __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
#define HZ_FUNCSIG __func__
#else
#define HZ_FUNCSIG "HZ_FUNCSIG unknown!"
#endif

#define HZ_PROFILE_BEGIN_SESSION(name, filepath) ::Hazel::Instrumentor::Get().BeginSession(name, filepath)//开始一个会话
#define HZ_PROFILE_END_SESSION() ::Hazel::Instrumentor::Get().EndSession()//结束一个会话
#define HZ_PROFILE_SCOPE(name) ::Hazel::InstrumentationTimer timer##__LINE__(name);//创建一个测试计时器
#define HZ_PROFILE_FUNCTION() HZ_PROFILE_SCOPE(HZ_FUNCSIG)//获取函数签名
#else
#define HZ_PROFILE_BEGIN_SESSION(name, filepath)
#define HZ_PROFILE_END_SESSION()
#define HZ_PROFILE_SCOPE(name)
#define HZ_PROFILE_FUNCTION()
#endif