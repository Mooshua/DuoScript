// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_MODULEINSTANCE_H
#define DUOSCRIPT_MODULEINSTANCE_H

#include <uv.h>

#include "Loop/Loop.h"
#include "IModule.h"
#include "ControllerManagerSink.h"

class ModuleInstance : public virtual IDuoServices
{
	friend class ModuleManager;
public:
	ModuleInstance();
	~ModuleInstance();

public:	//	IDuoServices
	IScriptControllerManager* ScriptController() override { return &controller_manager; };
	virtual ILoop* Loop() { return &g_Loop; }

protected:
	ISmmPlugin* plugin;
	ControllerManagerSink controller_manager;
};


#endif //DUOSCRIPT_MODULEINSTANCE_H
