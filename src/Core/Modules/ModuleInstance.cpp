// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "ModuleInstance.h"
#include "Script/ScriptVM.h"

ModuleInstance::ModuleInstance()
	: controller_manager(ControllerManagerSink(g_ScriptVM.controllers))
{

}

ModuleInstance::~ModuleInstance()
{

}

