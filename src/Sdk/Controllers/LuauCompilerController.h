// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#ifndef DUOSCRIPT_LUAUCOMPILERCONTROLLER_H
#define DUOSCRIPT_LUAUCOMPILERCONTROLLER_H

#include <IScriptController.h>

class LuauCompilerState
{
public:

};

class LuauCompilerController : public IScriptController<NoEntity>
{
public:
	LuauCompilerController()
	{
		CONTROLLER_NAME(LuauCompiler);

		CONTROLLER_STATIC_METHOD(TryCompile, &LuauCompilerController::TryCompile);
	}

	IScriptResult* TryCompile(IScriptCall* args);
};

extern LuauCompilerController g_LuauCompilerController;

#endif //DUOSCRIPT_LUAUCOMPILERCONTROLLER_H
