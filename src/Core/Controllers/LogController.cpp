// Copyright (c) 2024 Mooshua. All rights reserved.

#include "LogController.h"

IScriptResult *LogController::Info(IScriptCall *call)
{
	return this->Log(call, Log::SEV_INFO);
}

IScriptResult *LogController::Log(IScriptCall *call, ILogger::Severity severity)
{
	char message[256];
	if (!call->ArgString(1, message, sizeof(message)))
		return call->Error("Expected string for argument 1");

	//	TODO: Replace "Script" with isolate name
	g_Log.Message("Script", severity, "%s", message);

	return call->Return();
}
