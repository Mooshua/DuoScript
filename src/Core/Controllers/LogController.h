// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_LOGCONTROLLER_H
#define DUOSCRIPT_LOGCONTROLLER_H

#include <IScriptController.h>
#include <Logging/Log.h>

class LogController : public virtual IScriptController<void>
{
public:
	LogController()
	{
		CONTROLLER_NAME(Log);

		CONTROLLER_STATIC_METHOD(Info, &LogController::Info);
	}

	IScriptResult* Info(IScriptCall* call);

protected:
	IScriptResult* Log(IScriptCall* call, Log::Severity severity);
};


#endif //DUOSCRIPT_LOGCONTROLLER_H
