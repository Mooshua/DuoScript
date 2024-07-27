// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "DuoModule.h"
#include "Threading/FiberController.h"
#include "Memory/PointerController.h"

MODULE_INIT(DuoLogicModule);

void DuoLogicModule::OnReady()
{
	Logic = new DuoLogic(_logger, _loop, _files);
	Logic->OnScriptReady(g_Duo->ScriptController());
}

