#pragma once

#include <stdint.h>
#include <assert.h>
#include <queue>
#include <thread>
#include <string>
#include <functional>

namespace uaudio
{
	namespace logger
	{
		typedef enum LogSeverity {
			LOGSEVERITY_INFO,
			LOGSEVERITY_WARNING,
			LOGSEVERITY_ERROR,
			LOGSEVERITY_ASSERT
		} LogSeverity;

		struct Message
		{
			std::string message;
			LogSeverity severity;
		};

		class Logger
		{
		public:
			Logger();
			~Logger();
			void Log(LogSeverity a_Severity, const char* a_Message, const char* a_File, int a_Line);
			void LogF(LogSeverity a_Severity, const char* a_Message, const char* a_File, int a_Line, ...);
			void PrintMessage(LogSeverity a_Severity, const char* a_Message, const char* a_File, int a_Line);
			void MessageQueue();
		private:
			bool m_Running = false;
			std::queue<Message> m_messages;
			std::thread m_thread;
		};
		extern Logger logger;

#define LOG(severity, message)\
do{\
	logger::logger.Log(severity, message, __FILE__, __LINE__);\
} while (0)

#define LOGF(severity, message, ...)\
do{\
	logger::logger.LogF(severity, message, __FILE__, __LINE__, __VA_ARGS__);\
} while (0)
	}
}