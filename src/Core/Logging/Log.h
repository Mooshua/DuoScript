// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_LOG_H
#define DUOSCRIPT_LOG_H

#include <cstdarg>

class Log
{
public:
	enum Severity
	{
		///	Inconsequential, but can be used to
		///	diagnose issues.
		SEV_DEBUG = 0,

		///	Normal spew
		SEV_INFO,

		///	Something is wrong, but we can continue
		///	running. Behavior may change
		SEV_WARN,

		///	Something is wrong, and we are in an undefined
		///	state. We cannot continue, behavior is unknown.
		SEV_ERROR,

		LEN_SEVERITY,
	};

	enum Status
	{
		STAT_GOOD = 0,

		STAT_FAIL,

		STAT_NOTE,

		LEN_STATUS,
	};

	///	Log the success/failure of a component
	void Component(const char* system, Status status, const char* fmt, ...);
	void ComponentEx(const char* system, Status status, const char* fmt, va_list args);

	///	Log a generic message
	void Message(const char* system, Severity severity, const char* fmt, ...);
	void MessageEx(const char* system, Severity severity, const char* fmt, va_list args);

private:
	//const char* ToColor(const char* string);
	const char* ToString(Status status);
	const char* ToString(Severity severity);
};

extern Log g_Log;


#endif //DUOSCRIPT_LOG_H
