// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#ifndef DUOSCRIPT_MAIN_H
#define DUOSCRIPT_MAIN_H

#include <uv.h>

#include "Core/Script/ScriptVM.h"
#include "Core/Loop/Loop.h"
#include "Core/Plugins/PluginManager.h"
#include "Core.h"

class Main
{
public:
	void Initialize();

	[[noreturn]] void Run();

	DuoCore core;
protected:
	PluginLoadResult* _plugin;
};

extern Main g_Main;


#endif //DUOSCRIPT_MAIN_H
