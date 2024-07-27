// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "LogController.h"

LogController g_LogController;

IScriptResult *LogController::Info(IScriptCall *call)
{
	return this->Log(call, Log::SEV_INFO);
}

IScriptResult *LogController::Debug(IScriptCall *call)
{
	return this->Log(call, Log::SEV_DEBUG);
}

IScriptResult *LogController::Warn(IScriptCall *call)
{
	return this->Log(call, Log::SEV_WARN);
}

IScriptResult *LogController::Error(IScriptCall *call)
{
	return this->Log(call, Log::SEV_ERROR);
}

IScriptResult *LogController::Log(IScriptCall *call, ILogger::Severity severity)
{
	std::string message;
	ARG_STRING(call, 1, message);

	char from[64] = "Script";
	call->ArgString(2, from, sizeof(from));

	//	TODO: Replace "Script" with isolate name
	g_Log.Message(from, severity, "%s", message.c_str());

	return call->Return();
}


