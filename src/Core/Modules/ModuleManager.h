// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_MODULEMANAGER_H
#define DUOSCRIPT_MODULEMANAGER_H


#include "IModule.h"

class ModuleManager : public virtual IModuleIntroducer
{
public:
	void LoadAllModules();
public:
	virtual void Hello(IModule* myself, PluginId* id);
};

extern ModuleManager g_ModuleOnboarding;


#endif //DUOSCRIPT_MODULEMANAGER_H
