// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#ifndef DUOSCRIPT_MODULEMANAGER_H
#define DUOSCRIPT_MODULEMANAGER_H

#include "IModule.h"
#include "Generic/IntMap.h"
#include "ModuleInstance.h"

class ModuleManager : public virtual IModuleIntroducer
{
public:
	ModuleManager();
public:
	bool LoadModule(const char* name);

	void LoadAllModules();

	bool UnloadModule(PluginId id);

	void UnloadAllModules();


public:	//	IModuleIntroducer
	virtual ILogger* GetLogger();
	virtual void Hello(IModule* myself, PluginId* id);
	virtual void Goodbye(IModule* myself, PluginId* id);


protected:
	ke::IntMap<ModuleInstance*> _modules;
};

extern ModuleManager g_ModuleManager;


#endif //DUOSCRIPT_MODULEMANAGER_H
