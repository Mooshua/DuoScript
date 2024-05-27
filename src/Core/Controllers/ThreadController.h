// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#ifndef DUOSCRIPT_THREADCONTROLLER_H
#define DUOSCRIPT_THREADCONTROLLER_H

#include "Loop/Loop.h"
#include "IScriptController.h"
#include "sourcehook.h"

class ThreadResumer
{
public:
	ThreadResumer(IFiberHandle* handle, uint64_t delay)
	{
		_handle = handle;

		IDelayHandle* delayer = g_Loop.NewDelay( SH_MEMBER(this, &ThreadResumer::OnResume) );
		delayer->TryDelay(delay);
	}

	void OnResume(IDelayHandle* arg)
	{
		if (_handle->Exists())
			_handle->Get()->Call(false);

		delete _handle;
		delete arg;
		delete this;
	}

	IFiberHandle* _handle;
};

class ThreadController : public virtual IScriptController<void>
{
public:
	ThreadController()
	{
		CONTROLLER_NAME(Thread);
		CONTROLLER_STATIC_METHOD(Sleep, &ThreadController::Sleep);
		CONTROLLER_STATIC_METHOD(Fiber, &ThreadController::Fiber);
	}

	IScriptResult* Sleep(IScriptCall* args);
	IScriptResult* Fiber(IScriptCall* args);
};

extern ThreadController g_ThreadController;

#endif //DUOSCRIPT_THREADCONTROLLER_H
