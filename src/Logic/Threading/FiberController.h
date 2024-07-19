// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#ifndef DUOSCRIPT_FIBERCONTROLLER_H
#define DUOSCRIPT_FIBERCONTROLLER_H

#include <IScriptController.h>
#include "LogicGlobals.h"

class FiberEntity
{
public:
	FiberEntity() = default;
	explicit FiberEntity(IFiberHandle* handle)
	: fiber(handle)
	{

	}
public:
	IFiberHandle* fiber;
};

class ThreadResumer
{
public:
	ThreadResumer(IFiberHandle* handle, uint64_t delay)
	{
		_handle = handle;

		IDelayHandle* delayer = g_DuoLoop->NewDelay( fastdelegate::MakeDelegate(this, &ThreadResumer::OnResume) );
		delayer->TryDelay(delay);
	}

	void OnResume(IDelayHandle* arg)
	{
		DuoScope(FiberController::SleepResume);

		IScriptFiber* fiber = _handle->Get();
		if (fiber)
			fiber->Call(false);

		delete _handle;
		delete arg;
		delete this;
	}

	IFiberHandle* _handle;
};


class FiberController : public IScriptController<FiberEntity>
{
public:
	FiberController()
	{
		CONTROLLER_NAME(Fiber);

		CONTROLLER_STATIC_METHOD(New, &FiberController::New);
		CONTROLLER_STATIC_METHOD(Await, &FiberController::Await);
		CONTROLLER_STATIC_METHOD(Sleep, &FiberController::Sleep);

		CONTROLLER_METHOD(Resume, &FiberController::Resume);
		CONTROLLER_METHOD(Depend, &FiberController::Depend);
	}

public:
	IScriptResult* New(IScriptCall* args);
	IScriptResult* Await(IScriptCall* args);

	IScriptResult* Sleep(IScriptCall* args);

	IScriptResult* Resume(FiberEntity* entity, IScriptCall* args);
	IScriptResult* Depend(FiberEntity* entity, IScriptCall* args);
};

extern FiberController g_FiberController;


#endif //DUOSCRIPT_FIBERCONTROLLER_H
