// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_PLUGINMANAGER_H
#define DUOSCRIPT_PLUGINMANAGER_H

class Plugin;

#include <string>
#include <vector>

#include "Loop/Loop.h"
#include "Plugin.h"
#include <IPlugin.h>

class PluginLoadResult
{
public:
	std::string name;

	///	True when the load is successful
	bool success;

	///	Error message if failure
	std::string error;

	///	When load is successful, this is a plugin object.
	Plugin* plugin;
};

class PluginManager : public virtual IPluginManager
{
	friend class Plugin;

public:
	PluginLoadResult* LoadPlugin(const char* name);

	PluginLoadResult* GetPlugin(int id);

	PluginLoadResult* FindPlugin(const char* name);

	void LoadAllPlugins();

protected:
	std::vector<PluginLoadResult*> _plugins;
};

extern PluginManager g_PluginManager;

#endif //DUOSCRIPT_PLUGINMANAGER_H
