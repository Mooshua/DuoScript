// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#ifndef DUOSCRIPT_MODULEINSTANCE_H
#define DUOSCRIPT_MODULEINSTANCE_H

#include <uv.h>

#include "Loop/Loop.h"
#include "IModule.h"
#include "ControllerManagerSink.h"
#include "Core.h"

class ModuleInstance : public virtual IDuoServices
{
	DuoCore* _core;

	friend class ModuleManager;
public:
	ModuleInstance(DuoCore* core);
	~ModuleInstance();

public:	//	IDuoServices
	IScriptControllerManager* ScriptController() override { return &controller_manager; };
	virtual ILoop* Loop() { return _core->Loop; }

protected:
	ISmmPlugin* plugin;
	ControllerManagerSink controller_manager;
};


#endif //DUOSCRIPT_MODULEINSTANCE_H
