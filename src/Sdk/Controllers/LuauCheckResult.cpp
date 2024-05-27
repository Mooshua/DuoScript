// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "LuauCheckResult.h"
#include <amtl/am-string.h>

LuauCheckResultController g_LuauCheckResult;

IScriptResult* LuauCheckResultController::GetErrorCount(LuauCheckResult *entity, IScriptCall *args)
{
	auto count = entity->result.errors.size();

	args->PushInt(count);
	return args->Return();
}

IScriptResult *LuauCheckResultController::GetError(LuauCheckResult *entity, IScriptCall *args)
{
	unsigned index;
	if (!args->ArgUnsigned(1, &index))
		return args->Error("Expected argument 1 to be an index!");

	if (0 >= index || index > entity->result.errors.size())
		return args->Error("Invalid error index %i (out of %i)", index, entity->result.errors.size());

	auto error = entity->result.errors[index - 1];

	args->PushInt(error.code());
	args->PushString(toString(error).c_str());
	args->PushString(error.moduleName.c_str());
	args->PushString(this->ToLocation(error.location).c_str());

	return args->Return();
}

std::string LuauCheckResultController::ToLocation(Luau::Location location)
{
	//	Multi-line
	if (location.begin.line != location.end.line)
		return ke::StringPrintf("%02i:%02i->%02i:%02i", location.begin.line, location.begin.column, location.end.line, location.end.column);
	return ke::StringPrintf("%02i:%02i", location.begin.line, location.begin.column, location.end.column);
}

