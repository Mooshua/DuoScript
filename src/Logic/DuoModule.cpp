// Copyright (c) 2024 Mooshua. All rights reserved.

#include "DuoModule.h"
#include "Threading/FiberController.h"

MODULE_INIT(Logic);

void Logic::OnReady()
{
	this->OnScriptReady(g_Duo->ScriptController());
}

void Logic::OnScriptReady(IScriptControllerManager *manager)
{
	manager->Register(&g_FiberController);
}
