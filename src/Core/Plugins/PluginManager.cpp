// Copyright (c) 2024 Mooshua. All rights reserved.

#include "PluginManager.h"
#include "Helpers/Path.h"
#include "Logging/Log.h"
#include "DuoMetamod.h"
#include "Files/Files.h"

PluginManager g_PluginManager;

PluginLoadResult* PluginManager::LoadPlugin(const char *name)
{
	char error[512];
	char path[256];

	duo::BuildPath(path, sizeof(path), "plugins/%s", name);

	PluginLoadResult* result = new PluginLoadResult();
	this->_plugins.push_back(result);

	result->name = std::string(name);

	Plugin* plugin = new Plugin(name, this, this->_plugins.size() - 1 /* index of our PluginLoadResult* */);


	if (!plugin->TryLoad(error, sizeof(error)))
	{
		//	Failure
		result->success = false;
		result->error = std::string(error);
		result->plugin = nullptr;

		g_Log.Message("PluginSys", Log::SEV_WARN, "Error loading plugin %s: %s", result->name.c_str(), result->error.c_str());

		return result;
	}

	result->success = true;
	result->error = std::string ();
	result->plugin = plugin;

	g_Log.Message("PluginSys", Log::SEV_INFO, "Loaded plugin %s", result->name.c_str());

	return result;
}

PluginLoadResult *PluginManager::GetPlugin(int id)
{
	if (id < 0 || id >= this->_plugins.size())
		return nullptr;

	return this->_plugins[id];
}

PluginLoadResult *PluginManager::FindPlugin(const char *name)
{
	for (int i = 0; i < this->_plugins.size(); ++i) {
		PluginLoadResult* plugin = this->_plugins[i];

		if (plugin == nullptr)
			continue;

		if (strcmp(name, plugin->name.c_str()) == 0)
			return plugin;
	}

	return nullptr;
}

void PluginManager::LoadAllPlugins()
{
	std::vector<std::string> plugin_files = std::vector<std::string>();
	g_Files.GetFiles(&plugin_files, "plugins");

	for (const std::string &item: plugin_files)
	{
		//	Are we already loaded?
		if (this->FindPlugin(item.c_str()) != nullptr)
			continue;

		//	Automatic load
		PluginLoadResult* plugin = this->LoadPlugin(item.c_str());

		if (!plugin->success)
		{
			g_Log.Message("PluginSys", Log::SEV_WARN, "Error loading plugin %s!", item.c_str());
			g_Log.Message("PluginSys", Log::SEV_WARN, "%s: %s", item.c_str(), plugin->error.c_str());
			continue;
		}

		plugin->plugin->Start();
		g_Log.Message("PluginSys", Log::SEV_DEBUG, "Loaded plugin %s", item.c_str());

	}
}
