#include "utils/Logger.h"

#include <cstdarg>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <format>
#include <windows.h>

uaudio::logger::Logger uaudio::logger::logger;

namespace uaudio
{
	namespace logger
	{
		char arr[5][10] =
		{
			"INFO",
			"WARNING",
			"ERROR",
			"ASSERT"
		};

		Logger::Logger()
		{
			m_Running = true;
			m_Thread = std::thread(&Logger::MessageQueue, this);
		}

		Logger::~Logger()
		{
			m_MessagesMutex.lock();
			while (!m_Messages.empty()) m_Messages.pop();
			m_MessagesMutex.unlock();
			m_Running = false;
			m_Thread.join();
		}

		void Logger::Log(LogSeverity a_Severity, const char* a_Message, const char* a_File, int a_Line)
		{
			PrintMessage(a_Severity, a_Message, a_File, a_Line);
		}

		void Logger::LogF(LogSeverity a_Severity, const char* a_Message, const char* a_File, int a_Line, ...)
		{
			va_list va_format_list;
			va_start(va_format_list, a_Line);

			size_t buffersize = vsnprintf(NULL, 0, a_Message, va_format_list) + 1;
			char* formatted_message = (char*)malloc(buffersize);
			vsnprintf(formatted_message, buffersize, a_Message, va_format_list);

			PrintMessage(a_Severity, formatted_message, a_File, a_Line);

			free(formatted_message);
		}

		void Logger::PrintMessage(LogSeverity a_Severity, const char* a_Message, const char* a_File, int a_Line)
		{
			struct tm lt;
			time_t t;
			t = time(NULL);
			localtime_s(&lt, &t);

			std::string message = std::format("[{0}] \"{1}\" on line {2}: \"{3}\"\n",
				arr[a_Severity],
				a_File,
				a_Line,
				a_Message);

			m_MessagesMutex.lock();
			m_Messages.push({ message, a_Severity });
			m_MessagesMutex.unlock();
		}

		void Logger::MessageQueue()
		{
			while (m_Running)
			{
				m_MessagesMutex.lock();
				if (m_Messages.size() > 0)
				{
					Message lm = m_Messages.front();
					m_Messages.pop();

					DWORD written = 0;
					WriteConsoleA(
						GetStdHandle(STD_OUTPUT_HANDLE),
						lm.message.c_str(),
						static_cast<DWORD>(lm.message.size()),
						&written,
						nullptr
					);
					if (m_LoggerCallback)
						m_LoggerCallback(lm);
					if (lm.severity == logger::LOGSEVERITY_ASSERT)
						assert(0 && "Logger assert, check log file for information");
				}
				m_MessagesMutex.unlock();
			}
		}
	}
}