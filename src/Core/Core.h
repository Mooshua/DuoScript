// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#ifndef DUOSCRIPT_CORE_H
#define DUOSCRIPT_CORE_H

#include "Logging/Log.h"
#include "Loop/Loop.h"
#include "Script/ScriptVM.h"
#include "Files/Files.h"
#include "Plugins/PluginManager.h"

class DuoCore
{
public:
	DuoCore();

public:
	PluginManager* PluginManager;
	ScriptVM* ScriptVM;
	Loop* Loop;
	Log* Log;
	Files* Files;
};


#endif //DUOSCRIPT_CORE_H
