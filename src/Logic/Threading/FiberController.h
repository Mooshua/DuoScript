// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_FIBERCONTROLLER_H
#define DUOSCRIPT_FIBERCONTROLLER_H

#include <IScriptController.h>

class FiberEntity
{
public:
	explicit FiberEntity(IFiberHandle* handle)
	: fiber(handle)
	{

	}
public:
	IFiberHandle* fiber;
};

class FiberController : public IScriptController<FiberEntity>
{
public:
	FiberController()
	{
		CONTROLLER_NAME(Fiber);

		CONTROLLER_STATIC_METHOD(New, &FiberController::New);
		CONTROLLER_STATIC_METHOD(Await, &FiberController::Await);

		CONTROLLER_METHOD(Resume, &FiberController::Resume);
	}

public:
	IScriptResult* New(IScriptCall* args);
	IScriptResult* Await(IScriptCall* args);

	IScriptResult* Resume(FiberEntity* entity, IScriptCall* args);
};

extern FiberController g_FiberController;


#endif //DUOSCRIPT_FIBERCONTROLLER_H
