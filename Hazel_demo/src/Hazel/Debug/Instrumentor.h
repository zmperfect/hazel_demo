#pragma once

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>

#include <string>
#include <thread>

namespace Hazel {

    using FloatingPointMicroseconds = std::chrono::duration<double, std::micro>;//΢��

    //�����õ���
    struct ProfileResult
    {
        std::string Name;//���Ե�����
        
        FloatingPointMicroseconds Start;//��ʼʱ��
        std::chrono::microseconds ElapsedTime;//������ʱ��
        std::thread::id ThreadID;//�̵߳�ID
    };

    struct InstrumentationSession//�Ự
    {
        std::string Name;//�Ự������
    };
    
    //���Ե���
    class Instrumentor
    {
    public:
        Instrumentor(const Instrumentor&) = delete;
        Instrumentor(Instrumentor&&) = delete;

        void BeginSession(const std::string& name, const std::string& filepath = "results.json")//��ʼһ���Ự
        {
            std::lock_guard lock(m_Mutex);//����
            if (m_CurrentSession) 
            {
                // ����Ѿ���һ���Ự�ˣ��Ǿ��Ƚ�������Ȼ���ٿ�ʼ�µĻỰ��
                // �����ĻỰ֮�䲻Ӧ���м������Ϊ����ϣ�������������Ŀ飬������ɢ�ҵġ�
                if (Log::GetCoreLogger())//�����־��Ϊ��
                {
                    HZ_CORE_ERROR("Instrumentor::BeginSession('{0}') when session '{1}' already open.", name, m_CurrentSession->Name);
                }
                InternalEndSession();//������ǰ�Ự
            }

            m_OutputStream.open(filepath);// �������
            if(m_OutputStream.is_open())
            {
                m_CurrentSession = new InstrumentationSession({ name });//����һ���µĻỰ
                WriteHeader();//д��ͷ��
            }
            else
            {
                if (Log::GetCoreLogger())//�����־��Ϊ��
                {
                    HZ_CORE_ERROR("Instrumentor could not open results file '{0}'.", filepath);
                }
            }
        }

        void EndSession()//����һ���Ự
        {
            std::lock_guard lock(m_Mutex);//����
            InternalEndSession();//������ǰ�Ự
        }

        void WriteProfile(const ProfileResult& result)//д����Խ��
        {
            std::stringstream json;//����һ���ַ�����

            //�����Խ��д��json�ַ�������
            json << std::setprecision(3) << std::fixed;//���þ���
            json << ",{";
			json << "\"cat\":\"function\",";
			json << "\"dur\":" << (result.ElapsedTime.count()) << ',';
            json << "\"name\":\"" << result.Name << "\",";
            json << "\"ph\":\"X\",";
            json << "\"pid\":0,";
            json << "\"tid\":" << result.ThreadID << ",";
            json << "\"ts\":" << result.Start.count();
            json << "}";

            std::lock_guard lock(m_Mutex);//����
            //�����ǰ�Ự��Ϊ��
            if (m_CurrentSession) 
            {
				m_OutputStream << json.str();//д����Խ��
                m_OutputStream.flush();//��ջ�����
            }
        }

        static Instrumentor& Get()//��ȡ�������ʵ��
        {
            static Instrumentor instance;
            return instance;
        }

    private:
        Instrumentor()
            : m_CurrentSession(nullptr)
        {
        }

        ~Instrumentor()
        {
            EndSession();//������ǰ�Ự
        }

        void WriteHeader()//д��ͷ��
        {
            m_OutputStream << "{\"otherData\": {},\"traceEvents\":[{}";
            m_OutputStream.flush();//��ջ�����
        }

        void WriteFooter()//д��β��
        {
            m_OutputStream << "]}";
            m_OutputStream.flush();//��ջ�����
        }

        //Note:�������Ѿ�����������µ����������
        void InternalEndSession()//������ǰ�Ự
        {
            if (m_CurrentSession) {
                WriteFooter();//д��β��
                m_OutputStream.close();//�ر������
                delete m_CurrentSession;//ɾ����ǰ�Ự
                m_CurrentSession = nullptr;
            }
        }

    private:
        std::mutex m_Mutex;//������
        InstrumentationSession* m_CurrentSession;//��ǰ�Ự
        std::ofstream m_OutputStream;//�����
    };

    class InstrumentationTimer//���Եļ�ʱ��
    {
    public:
        InstrumentationTimer(const char* name)
            : m_Name(name), m_Stopped(false)
        {
            m_StartTimepoint = std::chrono::steady_clock::now();//��ȡ��ǰʱ��
        }

        ~InstrumentationTimer()
        {
            if (!m_Stopped)
                Stop();
        }

        void Stop()
        {
            auto endTimepoint = std::chrono::steady_clock::now();//��ȡ��ǰʱ��
            auto highResStart = FloatingPointMicroseconds{ m_StartTimepoint.time_since_epoch() };//��ʼʱ��
            auto elapsedTime = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch() - std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch();//������ʱ��

            Instrumentor::Get().WriteProfile({ m_Name, highResStart, elapsedTime, std::this_thread::get_id() });//��ȡ�߳�ID��д����Խ��

            m_Stopped = true;
        }
    private:
        const char* m_Name;//���Ե�����
        std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;//��ʼʱ��
        bool m_Stopped;//�Ƿ�ֹͣ
    };
}

namespace InstrumentorUtils {

    template<size_t N>
    struct ChangeResult//�ı���
    {
        char Data[N];
    };

    template<size_t N, size_t K>
    constexpr auto CleanupOutputString(const char(&expr)[N], const char(&remove)[K])//��������ַ���
    {
        ChangeResult<N> result = {};//����һ���ı���

        size_t srcIndex = 0;//Դ����
        size_t dstIndex = 0;//Ŀ������
        while (srcIndex < N)//�����ַ���
        {
            size_t matchIndex = 0;//ƥ������

            while(matchIndex < K - 1 && srcIndex + matchIndex < N - 1 && expr[srcIndex + matchIndex] == Data[matchIndex])//�����ַ���
            {
                matchIndex++;//ƥ����������
            }

            if (matchIndex == K - 1)//���ƥ����������K-1
            {
                srcIndex += matchIndex;//Դ��������
            }
        }
        return result;//���ظı���
    }
}

#define HZ_PROFILE 0
#if HZ_PROFILE

    // Resolve which function signature macro will be used. Note that this only
    // is resolved when the (pre)compiler starts, so the syntax highlighting
    // could mark the wrong one in your editor!
    #if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
        #define HZ_FUNCSIG __PRETTY_FUNCTION__
    #elif defined(__DMC__) && (__DMC__ >= 0x810)
        #define HZ_FUNCSIG __PRETTY_FUNCTION__
    #elif (defined(__FUNCSIG__) || (_MSC_VER))
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
#define HZ_PROFILE_SCOPE_LINE2(name, line) constexpr auto fixedName##line = ::Hazel::InstrumentorUtils::CleanupOutputString(name, "__cdecl ");\
											   ::Hazel::InstrumentationTimer timer##line(fixedName##line.Data)//���Եļ�ʱ��
#define HZ_PROFILE_SCOPE_LINE(name, line) HZ_PROFILE_SCOPE_LINE2(name, line)//���Ե�������
#define HZ_PROFILE_SCOPE(name) HZ_PROFILE_SCOPE_LINE(name, __LINE__)//���Ե�������
    #define HZ_PROFILE_FUNCTION() HZ_PROFILE_SCOPE(HZ_FUNCSIG)//��ȡ����ǩ��
#else
    #define HZ_PROFILE_BEGIN_SESSION(name, filepath)
    #define HZ_PROFILE_END_SESSION()
    #define HZ_PROFILE_SCOPE(name)
    #define HZ_PROFILE_FUNCTION()
#endif