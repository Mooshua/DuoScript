// Copyright (c) 2024 Mooshua. All rights reserved.

#include "DuoMetamod.h"
#include "ModuleManager.h"
#include "Logging/Log.h"
#include "Files/Files.h"
#include "Helpers/Path.h"

ModuleManager g_ModuleManager;

void ModuleManager::Hello(IModule *myself, PluginId* id)
{
	g_Log.Message("Duo Modules", Log::SEV_DEBUG,
				  "Discovered module '%s'", myself->GetName());
	ModuleInstance* instance;

	//	Have we registered this pluginid?
	auto lookup = _modules.find(*id);
	if (lookup.found()) {
		//	An instance was already found?
		//	likely caused by reload or some shenanigans.
		instance = lookup->value;
	} else {
		//	Create a new instance!
		instance = new ModuleInstance();
		_modules.add(_modules.findForAdd(*id), *id, instance);
	}

	instance->plugin = myself;

	//	Provide the plugin's service collection
	//	this allows it to grab the sinks that were created for it.
	myself->DuoLoad(instance);

	//	Todo: Any better place to start the module?
	myself->OnReady();
}



ModuleManager::ModuleManager()
{
	_modules.init();
}

ILogger *ModuleManager::GetLogger()
{
	return &g_Log;
}

void ModuleManager::LoadAllModules()
{
	std::vector<std::string> plugin_files = std::vector<std::string>();
	g_Files.GetFiles(&plugin_files, "modules");

	for (const std::string &item: plugin_files)
	{
		this->LoadModule(item.c_str());
	}
}

bool ModuleManager::LoadModule(const char *name)
{
	char path[256];
	char error[2048];
	bool already;

	duo::BuildPath(path, sizeof(path), "modules/%s", name);
	g_Log.Message("Duo Modules", ILogger::SEV_INFO, "Loading module %s", path);

	PluginId id = g_MetamodPluginManager->Load(
			path, g_PLID, already, error, sizeof(error));

	if (id == Pl_BadLoad)
	{
		//	Error loading module
		g_Log.Message("Duo Modules", ILogger::SEV_ERROR, "Failed loading module %s!", path);
		return false;
	}

	if (already)
	{
		g_Log.Message("Duo Modules", ILogger::SEV_WARN, "Attempted to load already loaded plugin %s", path);
		return true;	//	technically a success?
	}
}
