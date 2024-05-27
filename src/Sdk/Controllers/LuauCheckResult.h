// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_LUAUCHECKRESULT_H
#define DUOSCRIPT_LUAUCHECKRESULT_H

#include <IScriptController.h>
#include "Luau/Frontend.h"

class LuauCheckResult
{
public:
	Luau::CheckResult result;
};

class LuauCheckResultController : public IScriptController<LuauCheckResult>
{
	friend class LuauFrontendController;
public:
	LuauCheckResultController()
	{
		CONTROLLER_NAME(LuauCheckResult);

		CONTROLLER_GETTER(ErrorCount, &LuauCheckResultController::GetErrorCount);
		CONTROLLER_METHOD(GetError, &LuauCheckResultController::GetError);
	}

public:
	IScriptResult* GetErrorCount(LuauCheckResult* entity, IScriptCall* args);
	IScriptResult* GetError(LuauCheckResult* entity, IScriptCall* args);

protected:
	std::string ToLocation(Luau::Location location);
};

extern LuauCheckResultController g_LuauCheckResult;

#endif //DUOSCRIPT_LUAUCHECKRESULT_H
