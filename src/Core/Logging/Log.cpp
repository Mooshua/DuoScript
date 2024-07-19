// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include <cstdarg>
#include <iostream>
#include <amtl/am-string.h>
#include <Generic/StringMap.h>
#include <IProfiler.h>

#include "Log.h"

#define AnsiEscape "\x1B["

#ifndef DUO_INDEPENDENT

	#define ColorBlack AnsiEscape "30m"
	#define ColorWhite AnsiEscape "34m"
	#define ColorRed AnsiEscape "97m"
	#define ColorOrange AnsiEscape "32m"
	#define ColorYellow AnsiEscape "36m"
	#define ColorLime AnsiEscape "31m"
	#define ColorBrightGreen AnsiEscape "35m"
	#define ColorGreen AnsiEscape "33m"
	#define ColorTeal AnsiEscape "37m"
	#define ColorSky AnsiEscape "90m"
	#define ColorLightBlue AnsiEscape "94m"
	#define ColorBlue AnsiEscape "92m"
	#define ColorIndigo AnsiEscape "96m"
	#define ColorPurple AnsiEscape "91m"
	#define ColorPink AnsiEscape "95m"
	#define ColorRose AnsiEscape "93m"

	#define Reset ColorWhite

#else

	#define ColorBlack AnsiEscape "30m"
	#define ColorWhite AnsiEscape "97m"
	#define ColorRed AnsiEscape "91m"
	#define ColorOrange AnsiEscape "33m"
	#define ColorYellow AnsiEscape "93m"
	#define ColorLime AnsiEscape "92m"
	#define ColorBrightGreen AnsiEscape "32m"
	#define ColorGreen AnsiEscape "32m"
	#define ColorTeal AnsiEscape "36m"
	#define ColorSky AnsiEscape "96m"
	#define ColorLightBlue AnsiEscape "94m"
	#define ColorBlue AnsiEscape "34m"
	#define ColorIndigo AnsiEscape "35m"
	#define ColorPurple AnsiEscape "35m"
	#define ColorPink AnsiEscape "95m"
	#define ColorRose AnsiEscape "31m"

	#define Reset AnsiEscape "0m"

#endif


#define PRINT_IMPL(message) \
	/*if (g_Engine == nullptr)*/  \
        DuoScope(Log::PrintImpl); \
		std::cout << message;   \
	/*else*/ \
		/*g_Engine->LogPrint( message )*/;

Log g_Log;

void Log::Blank(const char *fmt, ...)
{
	std::va_list args;
	va_start(args, fmt);
		this->BlankEx(fmt, args);
	va_end(args);
}

void Log::Message(const char *system, Log::Severity severity, const char *fmt, ...)
{
	std::va_list args;
	va_start(args, fmt);
		this->MessageEx(system, severity, fmt, args);
	va_end(args);
}

void Log::Component(const char *system, Log::Status status, const char *fmt, ...)
{
	std::va_list args;
	va_start(args, fmt);
		this->ComponentEx(system, status, fmt, args);
	va_end(args);
}

void Log::Tip(const char *system, const char *fmt, ...)
{
	std::va_list args;
	va_start(args, fmt);
		this->TipEx(system, fmt, args);
	va_end(args);
}

void Log::ComponentEx(const char *system, Log::Status status, const char *fmt, va_list args)
{
	DuoScope(Log::Component);

	std::string inner_message = ke::StringPrintfVa(fmt, args);
	this->Sanitize(inner_message);

	std::string message = ke::StringPrintf(Reset "* %s init %15s " Reset "| %s\n" ,
					this->ToString(status), system, inner_message.c_str());

#if 0
	g_Engine->LogPrint(
			ColorBlack "Black "
			ColorWhite "White "
			ColorRed "Red "
			ColorOrange "Orange "
			ColorYellow "Yellow "
			ColorLime "Lime "
			ColorBrightGreen "BrightGreen "
			ColorGreen "Green "
			ColorTeal "Teal "
			ColorSky "Sky "
			ColorLightBlue "LightBlue "
			ColorBlue "Blue "
			ColorIndigo "Indigo "
			ColorPurple "Purple "
			ColorPink "Pink "
			ColorRose "Rose "
			);
#endif

	PRINT_IMPL( message.c_str() )
}

void Log::MessageEx(const char *system, Log::Severity severity, const char *fmt, va_list args)
{
	DuoScope(Log::Message);

	std::string inner_message = ke::StringPrintfVa(fmt, args);
	this->Sanitize(inner_message);

	std::string message = ke::StringPrintf(Reset "* %s %20s " Reset "| %s\n" ,
										   this->ToString(severity), system, inner_message.c_str());

	PRINT_IMPL( message.c_str() )
}

void Log::TipEx(const char *system, const char *fmt, va_list args)
{
	DuoScope(Log::Tip);

	const char* tip_thing = Reset "[" ColorYellow "tip!" Reset "]" ColorYellow;

	std::string inner_message = ke::StringPrintfVa(fmt, args);
	this->Sanitize(inner_message);

	std::string message = ke::StringPrintf(Reset "* %s tip %16s " Reset "| " ColorOrange " %s\n" Reset,
					tip_thing, system, inner_message.c_str());

	PRINT_IMPL( message.c_str() )
}

void Log::BlankEx(const char *fmt, va_list args)
{
	DuoScope(Log::Blank);

	std::string inner_message = ke::StringPrintfVa(fmt, args);
	this->Sanitize(inner_message);

	std::string message = ke::StringPrintf(Reset "* %27s " Reset "| %s\n" ,
										   "", inner_message.c_str());

	PRINT_IMPL( message.c_str() )
}

const char *Log::ToString(Log::Status status)
{
	static const char* values[LEN_STATUS]  = {
			//	Good
			Reset "[" ColorLime "okay" Reset "]" ColorLime,
			Reset "[" ColorRose "fail" Reset "]" ColorRose,
			Reset "[" ColorYellow "note" Reset "]" ColorYellow,
	};

	return values[status];
}

const char *Log::ToString(Log::Severity severity)
{
	static const char* values[LEN_SEVERITY]  = {
			Reset "[" ColorIndigo "dbug" Reset "]" ColorIndigo,
			Reset "[" ColorSky "info" Reset "]" ColorSky,
			Reset "[" ColorOrange "warn" Reset "]" ColorOrange,
			Reset "[" ColorRose "err!" Reset "]" ColorRose,
	};

	return values[severity];
}

//	Prevent ANSI escape sequences from being written to the console
void Log::Sanitize(std::string &line)
{
	DuoScope(Log::Sanitize);

	for (int i = 0; i < line.length(); i++)
	{
		if (line.data()[i] == '\x1')
			line.data()[i] = '^';
	}
}




/*
const char *Log::ToColor(const char *string)
{
	static const char* colors[5] = {
		ColorOrange,
		ColorYellow,
		ColorLime,
		ColorTeal,
		ColorSky,
	};

	size_t hash = ke::Hash(string);
	unsigned index = ((unsigned)hash) % 4;

	return colors[index];
}
*/