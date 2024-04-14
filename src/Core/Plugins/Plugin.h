// Copyright (c) 2024 Mooshua. All rights reserved.

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
	Plugin(const char* path, PluginManager* parent, int id);
	~Plugin();

	bool TryLoad(char* error, int maxlen);

	bool TryGetCodeResource(const char* name, std::string* results);

	void Start();

	void Stop();

public:	//	IPlugin
	IPluginResources* GetResources();


public:	//	IPluginResources
	virtual bool TryGetResource(const char* name, std::string* results);

	///	Get a list of resources contained in this plugin
	virtual void GetResources(std::vector<const char*>* names, const char* search = nullptr);

protected:

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
