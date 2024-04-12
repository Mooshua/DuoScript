// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_PATH_H
#define DUOSCRIPT_PATH_H

#include <ISmmAPI.h>
#include <amtl/os/am-path.h>
#include <amtl/am-platform.h>

extern ISmmAPI* g_SMAPI;

namespace duo
{
	inline void BuildPath(char* buffer, size_t maxlen, const char* fmt, ...)
	{
		const char* base = g_SMAPI->GetBaseDir();
		char formatted[512];

		va_list args;
		va_start(args, fmt);
			ke::path::FormatVa(formatted, sizeof(formatted), fmt, args);
		va_end(args);


		ke::path::Format(buffer, maxlen, "%s/addons/duo/%s", base, formatted);
	}

	inline void BuildPathEx(char* buffer, size_t maxlen, const char* fmt, va_list args)
	{
		const char* base = g_SMAPI->GetBaseDir();
		char formatted[512];

		ke::path::FormatVa(formatted, sizeof(formatted), fmt, args);
		ke::path::Format(buffer, maxlen, "%s/addons/duo/%s", base, formatted);
	}

}

#endif //DUOSCRIPT_PATH_H
