#pragma once

#include <stdint.h>
#include <assert.h>
#include <queue>
#include <thread>
#include <string>

namespace uaudio
{
	namespace logger
	{
		typedef enum LogSeverity {
			LOGSERVERITY_INFO,
			LOGSERVERITY_WARNING,
			LOGSERVERITY_ERROR,
			LOGSERVERITY_ASSERT
		} LogSeverity;

		class Logger
		{
		public:
			Logger();
			~Logger();
			void Log(LogSeverity a_Serverity, const char* a_Message, const char* a_File, int a_Line);
			void LogF(LogSeverity a_Serverity, const char* a_Message, const char* a_File, int a_Line, ...);
			void PrintMessage(LogSeverity a_Serverity, const char* a_Message, const char* a_File, int a_Line);
			void MessageQueue();
		private:
			bool m_Running = false;
			std::queue<std::string> m_messages;
			std::thread m_thread;
		};
		extern Logger logger;

#define LOG(severity, message)\
do{\
	logger::logger.Log(severity, message, __FILE__, __LINE__);\
	if (severity == logger::LOGSERVERITY_ASSERT)\
		assert(0 && "Logger assert, check log file for information");\
} while (0)

#define LOGF(severity, message, ...)\
do{\
	logger::logger.LogF(severity, message, __FILE__, __LINE__, __VA_ARGS__);\
	if (severity == logger::LOGSERVERITY_ASSERT)\
		assert(0 && "Logger assert, check log file for information");\
} while (0)
	}
}