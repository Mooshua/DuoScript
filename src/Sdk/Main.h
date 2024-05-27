// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_MAIN_H
#define DUOSCRIPT_MAIN_H

#include <uv.h>

#include "Core/Script/ScriptVM.h"
#include "Core/Loop/Loop.h"
#include "Core/Plugins/PluginManager.h"

class Main
{
public:
	void Initialize();
	void Run();

protected:
	PluginLoadResult* _plugin;
};

extern Main g_Main;


#endif //DUOSCRIPT_MAIN_H
