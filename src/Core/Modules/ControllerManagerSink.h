// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#ifndef DUOSCRIPT_CONTROLLERMANAGERSINK_H
#define DUOSCRIPT_CONTROLLERMANAGERSINK_H

#include <vector>

#include "IScriptController.h"

///	A sink that wraps an IScriptControllerManager, and tracks all
///	controller instances passed to us here.
class ControllerManagerSink : public virtual IScriptControllerManager
{
public:
	ControllerManagerSink(IScriptControllerManager* upstream);
	~ControllerManagerSink();

	///	Register a controller of type controller<Entity> for the global
	///	controller name "name". This controller will appear in all scripts.
	virtual IControllerInstance* Register(IBaseScriptController* controller);

	virtual void Destroy(IControllerInstance* instance);

private:
	IScriptControllerManager* upstream;
	std::vector<IControllerInstance*> controllers;
};


#endif //DUOSCRIPT_CONTROLLERMANAGERSINK_H
