// Copyright (c) 2024 Mooshua. All rights reserved.

#include "DuoModule.h"

MODULE_INIT(Cstrike);

void Cstrike::OnReady()
{
	this->OnScriptReady(g_Duo->ScriptController());
}

void Cstrike::OnScriptReady(IScriptControllerManager *manager)
{
}
