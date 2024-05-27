// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "PerformanceController.h"

PerformanceController g_PerformanceController;


IScriptResult *PerformanceController::Start(PerformanceEntity *entity, IScriptCall *call)
{
	entity->started = true;
	entity->started_at = uv_hrtime();
	entity->stopped_at = 0;

	return call->Return();
}

IScriptResult *PerformanceController::New(IScriptCall *call)
{
	PerformanceEntity entity = PerformanceEntity();

	return this->ReturnNew(call, &entity);
}

IScriptResult *PerformanceController::Stop(PerformanceEntity *entity, IScriptCall *call)
{
	//	if we never started, don't do anything.
	if (!entity->started)
		return call->Return();

	entity->started = false;
	entity->stopped_at = uv_hrtime();

	return call->Return();
}

IScriptResult *PerformanceController::GetNanoseconds(PerformanceEntity *entity, IScriptCall *call)
{
	if (entity->stopped_at == 0)
	{
		//	We're still running!
		call->PushNumber((uv_hrtime() - entity->started_at));
		return call->Return();
	}

	call->PushNumber((entity->stopped_at - entity->started_at));
	return call->Return();
}
