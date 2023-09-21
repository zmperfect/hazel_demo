#pragma once

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>

#include <thread>

namespace Hazel {
    //�����õ���
    struct ProfileResult
    {
        std::string Name;//���Ե�����
        long long Start, End;//��ʼʱ��ͽ���ʱ��
        uint32_t ThreadID;//�߳�ID
    };

    struct InstrumentationSession//�Ự
    {
        std::string Name;//�Ự������
    };
    
    //���Ե���
    class Instrumentor
    {
    private:
        InstrumentationSession* m_CurrentSession;//��ǰ�Ự
        std::ofstream m_OutputStream;//�����
        int m_ProfileCount;//���Ե�����
    public:
        Instrumentor()
            : m_CurrentSession(nullptr), m_ProfileCount(0)
        {
        }

        void BeginSession(const std::string& name, const std::string& filepath = "results.json")//��ʼһ���Ự
        {
            m_OutputStream.open(filepath);//�������
            WriteHeader();//д��ͷ��
            m_CurrentSession = new InstrumentationSession{ name };//����һ���Ự
        }

        void EndSession()//����һ���Ự
        {
            WriteFooter();//д��β��
            m_OutputStream.close();//�ر������
            delete m_CurrentSession;//ɾ���Ự
            m_CurrentSession = nullptr;
            m_ProfileCount = 0;
        }

        void WriteProfile(const ProfileResult& result)//д����Խ��
        {
            if (m_ProfileCount++ > 0)
                m_OutputStream << ",";

            std::string name = result.Name;//���Ե�����
            std::replace(name.begin(), name.end(), '"', '\'');//��˫�����滻Ϊ������

            //д����Խ��
            m_OutputStream << "{";
            m_OutputStream << "\"cat\":\"function\",";
            m_OutputStream << "\"dur\":" << (result.End - result.Start) << ',';
            m_OutputStream << "\"name\":\"" << name << "\",";
            m_OutputStream << "\"ph\":\"X\",";
            m_OutputStream << "\"pid\":0,";
            m_OutputStream << "\"tid\":" << result.ThreadID << ",";
            m_OutputStream << "\"ts\":" << result.Start;
            m_OutputStream << "}";
            
            m_OutputStream.flush();//��ջ�����
        }

        void WriteHeader()//д��ͷ��
        {
            m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
            m_OutputStream.flush();//��ջ�����
        }

        void WriteFooter()//д��β��
        {
            m_OutputStream << "]}";
            m_OutputStream.flush();//��ջ�����
        }

        static Instrumentor& Get()//��ȡ�������ʵ��
        {
            static Instrumentor instance;
            return instance;
        }
    };

    class InstrumentationTimer//���Եļ�ʱ��
    {
    public:
        InstrumentationTimer(const char* name)
            : m_Name(name), m_Stopped(false)
        {
            m_StartTimepoint = std::chrono::high_resolution_clock::now();//��ȡ��ǰʱ��
        }

        ~InstrumentationTimer()
        {
            if (!m_Stopped)
                Stop();
        }

        void Stop()
        {
            auto endTimepoint = std::chrono::high_resolution_clock::now();//��ȡ��ǰʱ��

            long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();//��ʼʱ��ת��
            long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();//����ʱ��ת��

            uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());//��ȡ�߳�ID
            Instrumentor::Get().WriteProfile({ m_Name, start, end, threadID });//д����Խ��

            m_Stopped = true;
        }
    private:
        const char* m_Name;//���Ե�����
        std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;//��ʼʱ��
        bool m_Stopped;//�Ƿ�ֹͣ
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

#define HZ_PROFILE_BEGIN_SESSION(name, filepath) ::Hazel::Instrumentor::Get().BeginSession(name, filepath)//��ʼһ���Ự
#define HZ_PROFILE_END_SESSION() ::Hazel::Instrumentor::Get().EndSession()//����һ���Ự
#define HZ_PROFILE_SCOPE(name) ::Hazel::InstrumentationTimer timer##__LINE__(name);//����һ�����Լ�ʱ��
#define HZ_PROFILE_FUNCTION() HZ_PROFILE_SCOPE(HZ_FUNCSIG)//��ȡ����ǩ��
#else
#define HZ_PROFILE_BEGIN_SESSION(name, filepath)
#define HZ_PROFILE_END_SESSION()
#define HZ_PROFILE_SCOPE(name)
#define HZ_PROFILE_FUNCTION()
#endif