// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_PATH_H
#define DUOSCRIPT_PATH_H

#include <amtl/os/am-path.h>
#include <amtl/am-platform.h>

#ifndef DUO_INDEPENDENT
#include <ISmmAPI.h>
extern ISmmAPI* g_SMAPI;
#else
	//	https://stackoverflow.com/questions/143174/how-do-i-get-the-directory-that-a-program-is-running-from
	#include <cstdio>  /* defines FILENAME_MAX */
	#ifdef KE_WINDOWS
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
	#else
		#include <unistd.h>
	#endif
#endif

namespace duo
{
	inline void BuildPathEx(char* buffer, size_t maxlen, const char* fmt, va_list args)
	{
#ifdef DUO_INDEPENDENT
		char base[FILENAME_MAX];
	#ifdef KE_WINDOWS
		GetModuleFileNameA(NULL, base, sizeof(base));
	#else
		readlink("/proc/self/exe", base, sizeof(base));
	#endif
#else
		const char* base = g_SMAPI->GetBaseDir();
#endif

#ifdef DUO_INDEPENDENT
		const char* format = "%s/%s";
#else
		const char* format = "%s/addons/duo/%s";
#endif
		char formatted[512];

		ke::path::FormatVa(formatted, sizeof(formatted), fmt, args);
		ke::path::Format(buffer, maxlen, format, base, formatted);
	}

	inline void BuildPath(char* buffer, size_t maxlen, const char* fmt, ...)
	{
		//const char* base = g_SMAPI->GetBaseDir();
		//char formatted[512];

		va_list args;
		va_start(args, fmt);
			//ke::path::FormatVa(formatted, sizeof(formatted), fmt, args);
			return BuildPathEx(buffer, maxlen, fmt, args);
		va_end(args);


		//ke::path::Format(buffer, maxlen, "%s/addons/duo/%s", base, formatted);
	}

}

#endif //DUOSCRIPT_PATH_H
