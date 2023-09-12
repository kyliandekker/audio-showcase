#include "utils/Logger.h"

#include <cstdarg>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <format>

uaudio::logger::Logger uaudio::logger::logger;

namespace uaudio
{
	namespace logger
	{
		char arr[5][10] =
		{
			"INFO",
			"MINOR",
			"MEDIUM",
			"HIGH",
			"ASSERT"
		};

		Logger::Logger()
		{
			m_Running = true;
			m_thread = std::thread(&Logger::MessageQueue, this);
		}

		Logger::~Logger()
		{
			m_Running = false;
			m_thread.join();
		}

		void Logger::Log(LogSeverity a_serverity, const char* a_message, const char* a_file, int a_line)
		{
			PrintMessage(a_serverity, a_message, a_file, a_line);
		}

		void Logger::LogF(LogSeverity a_serverity, const char* a_message, const char* a_file, int a_line, ...)
		{
			va_list va_format_list;
			va_start(va_format_list, a_line);

			size_t buffersize = vsnprintf(NULL, 0, a_message, va_format_list) + 1;
			char* formatted_message = (char*)malloc(buffersize);
			vsnprintf(formatted_message, buffersize, a_message, va_format_list);

			PrintMessage(a_serverity, formatted_message, a_file, a_line);

			free(formatted_message);
		}

		void Logger::PrintMessage(LogSeverity a_serverity, const char* a_message, const char* a_file, int a_line)
		{
			struct tm lt;
			time_t t;
			t = time(NULL);
			localtime_s(&lt, &t);

			std::string message = std::format("[{0}] {1} - File: {2} on line {3}.\n",
				arr[a_serverity],
				a_message,
				a_file,
				a_line);

			m_messages.push(message);
		}

		void Logger::MessageQueue()
		{
			while (m_Running)
			{
				if (m_messages.size() > 0)
				{
					std::string lm = m_messages.front();
					m_messages.pop();

					printf(lm.c_str());
				}
			}
		}
	}
}