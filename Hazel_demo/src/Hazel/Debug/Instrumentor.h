#pragma once

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>

#include <string>
#include <thread>

namespace Hazel {

    using FloatingPointMicroseconds = std::chrono::duration<double, std::micro>;//微秒

    //测试用的类
    struct ProfileResult
    {
        std::string Name;//测试的名字
        
        FloatingPointMicroseconds Start;//开始时间
        std::chrono::microseconds ElapsedTime;//经过的时间
        std::thread::id ThreadID;//线程的ID
    };

    struct InstrumentationSession//会话
    {
        std::string Name;//会话的名字
    };
    
    //测试的类
    class Instrumentor
    {
    private:
        std::mutex m_Mutex;//互斥量
        InstrumentationSession* m_CurrentSession;//当前会话
        std::ofstream m_OutputStream;//输出流
    public:
        Instrumentor()
            : m_CurrentSession(nullptr)
        {
        }

        void BeginSession(const std::string& name, const std::string& filepath = "results.json")//开始一个会话
        {
            std::lock_guard lock(m_Mutex);//加锁
            if (m_CurrentSession) {
                // 如果已经有一个会话了，那就先结束它，然后再开始新的会话。
                // 连续的会话之间不应该有间隔，因为我们希望它们是连续的块，而不是散乱的。
                if (Log::GetCoreLogger())//如果日志不为空
                {
                    HZ_CORE_ERROR("Instrumentor::BeginSession('{0}') when session '{1}' already open.", name, m_CurrentSession->Name);
                }
                InternalEndSession();//结束当前会话
            }

            m_OutputStream.open(filepath);// 打开输出流
            if(m_OutputStream.is_open()){
                m_CurrentSession = new InstrumentationSession({ name });//创建一个新的会话
                WriteHeader();//写入头部
            }else{
                if (Log::GetCoreLogger())//如果日志不为空
                {
                    HZ_CORE_ERROR("Instrumentor could not open results file '{0}'.", filepath);
                }
            }
        }

        void EndSession()//结束一个会话
        {
            std::lock_guard lock(m_Mutex);//加锁
            InternalEndSession();//结束当前会话
        }

        void WriteProfile(const ProfileResult& result)//写入测试结果
        {
            std::stringstream json;//创建一个字符串流

            std::string name = result.Name;//测试的名字
            std::replace(name.begin(), name.end(), '"', '\'');//将双引号替换为单引号

            //将测试结果写入json字符串流中
            json << std::setprecision(3) << std::fixed;//设置精度
            json << ",{";
			json << "\"cat\":\"function\",";
			json << "\"dur\":" << (result.ElapsedTime.count()) << ',';
            json << "\"name\":\"" << name << "\",";
            json << "\"ph\":\"X\",";
            json << "\"pid\":0,";
            json << "\"tid\":" << result.ThreadID << ",";
            json << "\"ts\":" << result.Start.count();
            json << "}";

            std::lock_guard lock(m_Mutex);//加锁
            //如果当前会话不为空
            if (m_CurrentSession) {
				m_OutputStream << json.str();//写入测试结果
                m_OutputStream.flush();//清空缓冲区
            }
        }

        static Instrumentor& Get()//获取测试类的实例
        {
            static Instrumentor instance;
            return instance;
        }

    private:

        void WriteHeader()//写入头部
        {
            m_OutputStream << "{\"otherData\": {},\"traceEvents\":[{}";
            m_OutputStream.flush();//清空缓冲区
        }

        void WriteFooter()//写入尾部
        {
            m_OutputStream << "]}";
            m_OutputStream.flush();//清空缓冲区
        }

        //Note:必须在已经加锁的情况下调用这个函数
        void InternalEndSession()//结束当前会话
        {
            if (m_CurrentSession) {
                WriteFooter();//写入尾部
                m_OutputStream.close();//关闭输出流
                delete m_CurrentSession;//删除当前会话
                m_CurrentSession = nullptr;
            }
        }

    };

    class InstrumentationTimer//测试的计时器
    {
    public:
        InstrumentationTimer(const char* name)
            : m_Name(name), m_Stopped(false)
        {
            m_StartTimepoint = std::chrono::steady_clock::now();//获取当前时间
        }

        ~InstrumentationTimer()
        {
            if (!m_Stopped)
                Stop();
        }

        void Stop()
        {
            auto endTimepoint = std::chrono::steady_clock::now();//获取当前时间
            auto highResStart = FloatingPointMicroseconds{ m_StartTimepoint.time_since_epoch() };//开始时间
            auto elapsedTime = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch() - std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch();//经过的时间

            Instrumentor::Get().WriteProfile({ m_Name, highResStart, elapsedTime, std::this_thread::get_id() });//获取线程ID并写入测试结果

            m_Stopped = true;
        }
    private:
        const char* m_Name;//测试的名字
        std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;//开始时间
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