// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_LOOPCONTROLLER_H
#define DUOSCRIPT_LOOPCONTROLLER_H

#include "IScriptController.h"

class LoopController : IScriptController<void>
{
public:
	LoopController()
	{
		CONTROLLER_NAME(Loop);
		CONTROLLER_STATIC_METHOD(Sleep, &LoopController::Sleep);
	}

	IScriptResult* Sleep(IScriptCall* args);
};

#endif //DUOSCRIPT_LOOPCONTROLLER_H
