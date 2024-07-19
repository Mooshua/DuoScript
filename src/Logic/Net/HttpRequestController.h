// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#ifndef DUOSCRIPT_HTTPREQUESTCONTROLLER_H
#define DUOSCRIPT_HTTPREQUESTCONTROLLER_H

#include <IScriptController.h>
#include <httplib.h>

class HttpRequestEntity
{

};

class HttpRequestController : public virtual IScriptController<HttpRequestEntity>
{
public:
	HttpRequestController()
	{
		CONTROLLER_NAME(HttpRequest);
		CONTROLLER_STATIC_METHOD(New, &HttpRequestController::New);
	}

	IScriptResult *New(IScriptCall *call);

};

#endif //DUOSCRIPT_HTTPREQUESTCONTROLLER_H
