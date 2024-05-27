// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "DuoModule.h"

MODULE_INIT(Cstrike);

void Cstrike::OnReady()
{
	this->OnScriptReady(g_Duo->ScriptController());
}

void Cstrike::OnScriptReady(IScriptControllerManager *manager)
{
}
