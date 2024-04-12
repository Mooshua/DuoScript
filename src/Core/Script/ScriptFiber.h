// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_SCRIPTFIBER_H
#define DUOSCRIPT_SCRIPTFIBER_H

class ScriptFiber;

#include "ScriptVM.h"
#include "ScriptCall.h"

class ScriptFiber : public virtual IScriptFiber
{
	friend class ScriptIsolate;
public:
	ScriptFiber(ScriptIsolate* parent, lua_State *L, IScriptRef* thread_ref);
	~ScriptFiber();

public:
	///	True when the fiber is ready to begin another call
	virtual bool IsReady();

	virtual IFiberHandle* ToHandle();

	///	Begin a call to the specified method
	virtual bool TrySetup(IScriptMethod* method, IScriptInvoke** args);

	virtual bool TryContinue(IScriptInvoke** args = nullptr);


	///	Invoke this isolate
	virtual IScriptCall* Call(bool use);

public:
	ScriptCall call;
	ScriptIsolate *parent;
	lua_State *L;

protected:
	bool first_invoke;
	IScriptRef* thread_reference;
	//	Unique ID assigned to us by the isolate
	int fiber_id;
};


#endif //DUOSCRIPT_SCRIPTFIBER_H
