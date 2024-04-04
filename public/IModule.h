// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_IMODULE_H
#define DUOSCRIPT_IMODULE_H

#include <IScriptController.h>

class IModule
{
public:
	///	Load this module and begin providing functionality.
	///	Do not register controllers--that is called in
	///	OnRegisterControllers().
	virtual void OnLoad() = 0;

	///	End functionality and remove all references to our
	///	library or heap. The module may be unloaded at any time
	///	after this is invoked.
	virtual void OnDestroy() = 0;

	///	Called exactly once to tell this module to
	///	register any controllers to the scripting system
	///	Do not worry about unregistering them--a sink is provided,
	///	which will handle your controllers' unload.
	virtual void OnRegisterControllers(IScriptControllerManager* manager) = 0;
};

#endif //DUOSCRIPT_IMODULE_H
