// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_LOG_H
#define DUOSCRIPT_LOG_H

#include <ILogger.h>

class Log : public virtual ILogger
{
public:
	///	Log the success/failure of a component
	virtual void Component(const char* system, ILogger::Status status, const char* fmt, ...);
	virtual void ComponentEx(const char* system, ILogger::Status status, const char* fmt, va_list args);

	///	Log a generic message
	virtual void Message(const char* system, ILogger::Severity severity, const char* fmt, ...);
	virtual void MessageEx(const char* system, ILogger::Severity severity, const char* fmt, va_list args);

	virtual void Tip(const char* system, const char* fmt, ...);
	virtual void TipEx(const char* system, const char* fmt, va_list args);
private:
	//const char* ToColor(const char* string);
	const char* ToString(Status status);
	const char* ToString(Severity severity);
};

extern Log g_Log;


#endif //DUOSCRIPT_LOG_H
