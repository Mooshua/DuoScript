// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#ifndef DUOSCRIPT_LUAUCHECKRESULT_H
#define DUOSCRIPT_LUAUCHECKRESULT_H

#include <IScriptController.h>
#include "Luau/Frontend.h"

class LuauCheckResult
{
public:
	Luau::CheckResult result;
	std::string name;
};

class LuauCheckResultController : public IScriptController<LuauCheckResult>
{
	friend class LuauFrontendController;
public:
	LuauCheckResultController()
	{
		CONTROLLER_NAME(LuauCheckResult);

		CONTROLLER_AUTO_GETTER(Name, std::string, &LuauCheckResult::name);
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
