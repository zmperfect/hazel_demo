#include "hzpch.h"
#include "Hazel/Core/Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Hazel {

	Ref<spdlog::logger>  Log::s_CoreLogger;
	Ref<spdlog::logger>  Log::s_ClientLogger;

	void Log::Init()
	{
		std::vector<spdlog::sink_ptr> logSinks;//新建一个日志输出流的容器
		logSinks.emplace_back(CreateRef<spdlog::sinks::stdout_color_sink_mt>());//将控制台输出流加入容器
		logSinks.emplace_back(CreateRef<spdlog::sinks::basic_file_sink_mt>("Hazel.log", true));//将文件输出流加入容器

		logSinks[0]->set_pattern("%^[%T] %n: %v%$");//设置控制台输出格式
		logSinks[1]->set_pattern("[%T] [%l] %n: %v");//设置文件输出格式

		s_CoreLogger = CreateRef<spdlog::logger>("HAZEL", begin(logSinks), end(logSinks));//创建Core日志对象
		spdlog::register_logger(s_CoreLogger);//注册日志对象
		s_CoreLogger->set_level(spdlog::level::trace);
		s_CoreLogger->flush_on(spdlog::level::trace);//设置日志对象的刷新等级

		s_ClientLogger = CreateRef<spdlog::logger>("APP", begin(logSinks), end(logSinks));//创建Client日志对象
		spdlog::register_logger(s_ClientLogger);//注册日志对象
		s_ClientLogger->set_level(spdlog::level::trace);
		s_ClientLogger->flush_on(spdlog::level::trace);//设置日志对象的刷新等级
	}
}
