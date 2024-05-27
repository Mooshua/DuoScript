// Copyright (c) 2024 Mooshua. All rights reserved.

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
