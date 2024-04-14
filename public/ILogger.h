// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_ILOGGER_H
#define DUOSCRIPT_ILOGGER_H

#include <cstdarg>

class ILogger
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
	virtual void Component(const char *system, Status status, const char *fmt, ...) = 0;
	virtual void ComponentEx(const char *system, Status status, const char *fmt, va_list args) = 0;

	///	Log a generic message
	virtual void Message(const char *system, Severity severity, const char *fmt, ...) = 0;
	virtual void MessageEx(const char *system, Severity severity, const char *fmt, va_list args) = 0;

	virtual void Tip(const char *system, const char *fmt, ...) = 0;
	virtual void TipEx(const char *system, const char *fmt, va_list args) = 0;
};

#endif //DUOSCRIPT_ILOGGER_H
