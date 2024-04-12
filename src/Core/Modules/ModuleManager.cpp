// Copyright (c) 2024 Mooshua. All rights reserved.

#include "ModuleManager.h"
#include "Logging/Log.h"

ModuleManager g_ModuleOnboarding;

void ModuleManager::Hello(IModule *myself, PluginId* id)
{
	g_Log.Message("Duo Mod", Log::SEV_INFO, "Recived Hello from %i!", id);
}

void ModuleManager::LoadAllModules()
{

}
