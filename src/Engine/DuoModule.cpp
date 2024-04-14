// Copyright (c) 2024 Mooshua. All rights reserved.

#include "DuoModule.h"

MODULE_INIT(Engine);

void Engine::OnReady()
{
	this->OnScriptReady(g_Duo->ScriptController());
}

void Engine::OnScriptReady(IScriptControllerManager *manager)
{
}
