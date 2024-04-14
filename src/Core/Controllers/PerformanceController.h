// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_PERFORMANCECONTROLLER_H
#define DUOSCRIPT_PERFORMANCECONTROLLER_H

#include "uv.h"
#include <IScript.h>
#include <IScriptController.h>

class PerformanceEntity
{
public:
	bool started = false;
	uint64_t started_at = 0;
	uint64_t stopped_at = 0;
};

class PerformanceController : public virtual IScriptController<PerformanceEntity>
{
public:
	PerformanceController()
	{
		CONTROLLER_NAME(Performance);

		CONTROLLER_STATIC_METHOD(New, &PerformanceController::New);
		CONTROLLER_METHOD(Start, &PerformanceController::Start);
		CONTROLLER_METHOD(Stop, &PerformanceController::Stop);

		CONTROLLER_GETTER(ElapsedNanoseconds, &PerformanceController::GetNanoseconds);
	}

public:
	IScriptResult* New(IScriptCall* call);

public:
	IScriptResult* Start(PerformanceEntity* entity, IScriptCall* call);
	IScriptResult* Stop(PerformanceEntity* entity, IScriptCall* call);

	IScriptResult* GetNanoseconds(PerformanceEntity* entity, IScriptCall* call);
};


#endif //DUOSCRIPT_PERFORMANCECONTROLLER_H
