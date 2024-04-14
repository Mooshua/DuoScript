// Copyright (c) 2024 Mooshua. All rights reserved.

#include <cstdarg>
#include <amtl/am-string.h>
#include <Generic/StringMap.h>

#include "Log.h"
#include "DuoMetamod.h"

#define AnsiEscape "\x1B["

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

Log g_Log;

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
	char inner_message[4096];
	char message[4096];

	ke::SafeVsprintf(inner_message, sizeof(inner_message), fmt, args);
	ke::SafeSprintf(message, sizeof(message), Reset "* %s init %12s " Reset "| %s \n" ,
					this->ToString(status), system, inner_message);

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
	if (g_Engine == nullptr)
		printf("%s",message);
	else
		g_Engine->LogPrint(message);
}

void Log::MessageEx(const char *system, Log::Severity severity, const char *fmt, va_list args)
{
	char inner_message[4096];
	char message[4096];

	ke::SafeVsprintf(inner_message, sizeof(inner_message), fmt, args);
	ke::SafeSprintf(message, sizeof(message), Reset "* %s %17s " Reset "| %s \n" ,
					this->ToString(severity), system, inner_message);

	if (g_Engine == nullptr)
		printf("%s",message);
	else
		g_Engine->LogPrint(message);
}

void Log::TipEx(const char *system, const char *fmt, va_list args)
{
	char inner_message[4096];
	char message[4096];

	const char* tip_thing = Reset "[" ColorYellow "tip!" Reset "]" ColorYellow;

	ke::SafeVsprintf(inner_message, sizeof(inner_message), fmt, args);
	ke::SafeSprintf(message, sizeof(message), Reset "* %s tip %13s " Reset "| " ColorOrange " %s \n" Reset,
					tip_thing, system, inner_message);

	if (g_Engine == nullptr)
		printf("%s",message);
	else
		g_Engine->LogPrint(message);
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