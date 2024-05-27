// Copyright (c) 2024 Mooshua. All rights reserved.

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
	char message[256];
	if (!call->ArgString(1, message, sizeof(message)))
		return call->Error("Expected string for argument 1");

	char from[64] = "Script";
	call->ArgString(2, from, sizeof(from));

	//	TODO: Replace "Script" with isolate name
	g_Log.Message(from, severity, "%s", message);

	return call->Return();
}


