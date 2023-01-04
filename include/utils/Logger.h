#pragma once

#include <stdint.h>
#include <assert.h>

typedef enum LogSeverity {
	LOGSERVERITY_INFO,
	LOGSERVERITY_WARNING,
	LOGSERVERITY_ERROR,
	LOGSERVERITY_ASSERT
} LogSeverity;

#define LOG(severity, message)\
do{\
	Log(severity, message, __FILE__, __LINE__);\
	if (severity == LOGSERVERITY_ASSERT)\
		assert(0 && "Logger assert, check log file for information");\
} while (0)

#define LOGF(severity, message, ...)\
do{\
	LogF(severity, message, __FILE__, __LINE__, __VA_ARGS__);\
	if (severity == LOGSERVERITY_ASSERT)\
		assert(0 && "Logger assert, check log file for information");\
} while (0)

void Log(LogSeverity a_Serverity, const char* a_Message, const char* a_File, int a_Line);
void LogF(LogSeverity a_Serverity, const char* a_Message, const char* a_File, int a_Line, ...);
bool CheckSeverity(LogSeverity a_Serverity);
void PrintMessage(LogSeverity a_Serverity, const char* a_Message, const char* a_File, int a_Line);