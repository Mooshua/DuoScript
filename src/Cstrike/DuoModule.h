// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_DUOMODULE_H
#define DUOSCRIPT_DUOMODULE_H

#include <IModule.h>

MODULE_NEW(Cstrike)
{
	MODULE_BUILTINS;

	MODULE_NAME_SHORT("DuoCstrike");
	MODULE_NAME("Cstrike", "Game-specific support for Counter-Strike 2");
	MODULE_AUTHOR("Mooshua", "https://mooshua.net/");

	MODULE_VERSION("1.0.0");
	MODULE_LICENSE("MIT");

protected:
	void OnScriptReady(IScriptControllerManager* manager);
};


#endif //DUOSCRIPT_DUOMODULE_H
