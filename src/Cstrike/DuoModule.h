// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

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
