// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#ifndef DUOSCRIPT_PLUGINMANAGER_H
#define DUOSCRIPT_PLUGINMANAGER_H

class Plugin;

#include <string>
#include <vector>

#include "Loop/Loop.h"
#include "Plugin.h"
#include "Files/Files.h"
#include <IPlugin.h>
#include <ILogger.h>
#include "Script/ScriptVM.h"

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
	ScriptVM* _vm;
	ILogger* _log;
	Files* _files;

	friend class Plugin;
public:
	PluginManager(ILogger* log, Files* files, ScriptVM* vm)
	: _log(log)
	, _files(files)
	, _vm(vm)
	{
	}

	PluginLoadResult* LoadPluginInternal(const char* name, const char* path);

public:	// IPluginManager
	PluginLoadResult* LoadPlugin(const char* name);

	PluginLoadResult* GetPlugin(int id);

	PluginLoadResult* FindPlugin(const char* name);

	void LoadAllPlugins();

protected:
	std::vector<PluginLoadResult*> _plugins;
};

#endif //DUOSCRIPT_PLUGINMANAGER_H
