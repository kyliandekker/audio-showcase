#include "utils/Logger.h"

#include <cstdarg>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char arr[5][10] =
{
	"INFO:",
	"MINOR:",
	"MEDIUM:",
	"HIGH:",
	"ASSERT:"
};

void Log(LogSeverity a_serverity, const char* a_message, const char* a_file, int a_line)
{
	PrintMessage(a_serverity, a_message, a_file, a_line);
}

void LogF(LogSeverity a_serverity, const char* a_message, const char* a_file, int a_line, ...)
{
	va_list va_format_list;
	va_start(va_format_list, a_line);

	size_t buffersize = vsnprintf(NULL, 0, a_message, va_format_list) + 1;
	char* formatted_message = (char*)malloc(buffersize);
	vsnprintf(formatted_message, buffersize, a_message, va_format_list);

	PrintMessage(a_serverity, formatted_message, a_file, a_line);

	free(formatted_message);
}

void PrintMessage(LogSeverity a_serverity, const char* a_message, const char* a_file, int a_line)
{
	struct tm lt;
	time_t t;
	t = time(NULL);
	localtime_s(&lt, &t);

	printf("% s % s - File: % s on line % d\n",
		arr[a_serverity],
		a_message,
		a_file,
		a_line);
}