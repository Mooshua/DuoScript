// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#ifndef DUOSCRIPT_PLUGIN_H
#define DUOSCRIPT_PLUGIN_H

class PluginManager;

#include "PluginManager.h"
#include "Script/ScriptVM.h"
#include <IPlugin.h>
#include <zip.h>


class Plugin : public virtual IPlugin, public virtual IPluginResources
{
public:
	Plugin(ScriptVM* vm, const char* path, PluginManager* parent, int id);
	~Plugin();

	bool TryLoad(char* error, int maxlen);


	void Start();

	void Stop();

public:	//	IPlugin
	IPluginResources* GetResources();

public:	//	IPluginResources
	virtual const char* GetName(int* length);
	virtual ResourceType TryGetResource(const char* name, std::string* results);
	virtual bool TryGetCodeResource(const char* name, std::string* results);

	///	Get a list of resources contained in this plugin
	virtual void GetResources(std::vector<const char*>* names, const char* search = nullptr);

protected:

	std::string _path;
	std::string _name;

	int _id;
	PluginManager* _parent;

	bool _loaded;

	//	Entry point to the plugin
	IScriptMethod* _entryMethod;

	//	A handle to the zip file that holds this plugin
	zip_t* _file;

	//	The script VM that this plugin runs in
	ScriptIsolate* _isolate;
};


#endif //DUOSCRIPT_PLUGIN_H
