// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

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
	~ScriptFiber() override;

public:
	///	True when the fiber is ready to begin another call
	virtual bool IsReady();

	virtual IFiberHandle* ToHandle();

	///	Begin a call to the specified method
	virtual bool TrySetup(IScriptMethod* method, IScriptInvoke** args);

	virtual bool TryContinue(IScriptInvoke** args = nullptr);

	///	Kill this fiber with an error, without actually running the fiber.
	///	This does the same logic as an error within the fiber.
	virtual void Kill(const char* fmt, ...);

	///	Invoke this isolate
	virtual IScriptReturn* Call(bool use);

	virtual bool TryDepend(IScriptFiber* other);

protected:
	///	Report an error and unwind the stack
	void OnError(const char* error);

public:
	ScriptCall call;
	ScriptIsolate *parent;
	lua_State *L;

protected:
	bool first_invoke;
	IScriptRef* thread_reference;
	//	Unique ID assigned to us by the isolate
	int fiber_id;

	std::vector<IFiberHandle*> _dependants;
};


#endif //DUOSCRIPT_SCRIPTFIBER_H
