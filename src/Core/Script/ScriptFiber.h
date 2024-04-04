// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_SCRIPTFIBER_H
#define DUOSCRIPT_SCRIPTFIBER_H

class ScriptFiber;

#include "ScriptVM.h"
#include "ScriptCall.h"

class ScriptFiber : public virtual IScriptFiber
{
public:
	ScriptFiber(ScriptIsolate* parent);

public:
	///	True when the fiber is ready to begin another call
	virtual bool IsReady();

	///	Begin a call to the specified method
	virtual bool TrySetup(IScriptMethod* method);

	///	Invoke this isolate
	virtual IScriptCall* Call();

public:
	ScriptCall call;
	ScriptIsolate *parent;
	lua_State *L;
};


#endif //DUOSCRIPT_SCRIPTFIBER_H
