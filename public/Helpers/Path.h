// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under the Lesser GPL (see public/LICENSE.md), and defines an
// interface that may be linked to without distributing the linkee under a GPL license.

#ifndef DUOSCRIPT_PATH_H
#define DUOSCRIPT_PATH_H

#include <amtl/os/am-path.h>
#include <amtl/am-platform.h>
#include <filesystem>
#include <IProfiler.h>

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
	inline std::string ExecutablePath()
	{
		char base[FILENAME_MAX];
#ifdef KE_WINDOWS
		GetModuleFileNameA(NULL, base, sizeof(base));
#else
		readlink("/proc/self/exe", base, sizeof(base));
#endif

		return std::filesystem::path(base).parent_path().string();
	}

	inline void BuildPathEx(char* buffer, size_t maxlen, const char* fmt, va_list args)
	{
		DuoScope(Duo::BuildPath);

#ifdef DUO_INDEPENDENT
		const char* base = "";
#else
		const char* base = g_SMAPI->GetBaseDir();
#endif

#ifdef DUO_INDEPENDENT
		const char* format = "%s%s";
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
