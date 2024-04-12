// Copyright (c) 2024 Mooshua. All rights reserved.

#include "ModuleInstance.h"
#include "Script/ScriptVM.h"

ModuleInstance::ModuleInstance()
	: controller_manager(ControllerManagerSink(g_ScriptVM.controllers))
{

}
