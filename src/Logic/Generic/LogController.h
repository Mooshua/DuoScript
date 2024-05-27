// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#ifndef DUOSCRIPT_LOGCONTROLLER_H
#define DUOSCRIPT_LOGCONTROLLER_H

#include <IScriptController.h>
#include <Logging/Log.h>

class LogController : public virtual IScriptController<NoEntity>
{
public:
	LogController()
	{
		CONTROLLER_NAME(Log);

		CONTROLLER_STATIC_METHOD(Debug, &LogController::Debug);
		CONTROLLER_STATIC_METHOD(Info, &LogController::Info);
		CONTROLLER_STATIC_METHOD(Warn, &LogController::Warn);
		CONTROLLER_STATIC_METHOD(Error, &LogController::Error);

	}
	IScriptResult* Debug(IScriptCall* call);
	IScriptResult* Info(IScriptCall* call);
	IScriptResult* Warn(IScriptCall* call);
	IScriptResult* Error(IScriptCall* call);

protected:
	IScriptResult* Log(IScriptCall* call, Log::Severity severity);
};

extern LogController g_LogController;


#endif //DUOSCRIPT_LOGCONTROLLER_H
