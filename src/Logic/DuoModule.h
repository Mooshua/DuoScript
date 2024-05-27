// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#ifndef DUOSCRIPT_DUOMODULE_H
#define DUOSCRIPT_DUOMODULE_H

#include <IModule.h>

MODULE_NEW(Logic)
{
	MODULE_BUILTINS;

	MODULE_NAME_SHORT("DuoLogic");
	MODULE_NAME("Logic", "Universal DuoScript controllers");
	MODULE_AUTHOR("Mooshua", "https://mooshua.net/");

	MODULE_VERSION("1.0.0");
	MODULE_LICENSE("MIT");

protected:
	void OnScriptReady(IScriptControllerManager* manager);
};


#endif //DUOSCRIPT_DUOMODULE_H
