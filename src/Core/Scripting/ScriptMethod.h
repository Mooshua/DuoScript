// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_SCRIPTMETHOD_H
#define DUOSCRIPT_SCRIPTMETHOD_H


#include "IScriptMethod.h"
#include "lua.h"
#include "lualib.h"
#include "Script.h"
#include "ScriptIsolate.h"

class ScriptMethod : public IScriptMethod
{
public:
	ScriptMethod(lua_State *L, int ref)
	:	L(L)
	,	ref(ref)
	{}
	~ScriptMethod()
	{
		lua_unref(L, ref);
	}

	int AsReferenceId();

	virtual IScriptExecution* Invoke();

private:
	lua_State *L;
	int ref;
};

class ScriptExecution : public IScriptExecution
{
public:
	ScriptExecution(lua_State *coroutine, ScriptMethod *method)
	:	L(coroutine)
	,	method(method)
	{
		//	We SHOULD BE in a fresh lua_thread right now
		//	Push the coroutine method to the stack so when
		//	we call lua_resume we don't need to do anything fancy.
		lua_getref(L, method->AsReferenceId());
	}
	~ScriptExecution()
	{
		lua_close(L);
	}

	///	Determines if this execution can be resumed
	///	False on error, stop, or not started yet.
	virtual bool IsResumable();

	virtual IScriptCaller* Resume();

protected:

	///	Get the isolate that is currently running this script
	ScriptIsolate* GetIsolate()
	{
		return static_cast<ScriptIsolate *>(lua_getthreaddata(L));
	}

private:
	lua_State *L;
	ScriptMethod *method;
};

class ScriptCaller : public IScriptCaller
{
public:
	ScriptCaller(lua_State *L)
	: L(L)
	, arg_count(0)
	{

	}

public:
	virtual void PushNumber(double number);
	virtual void PushObject(IScriptObject* object);
	virtual void PushRef(IScriptRef* ref);

public:
	///	Execute this object.
	///	Returns false and writes to error if the
	///	script code throws an error.
	virtual bool TryExecute(IScriptError** error);

	///	Abandon this invocation and clean up our stack.
	virtual void Abandon();

public:
	lua_State *L;

private:
	int arg_count;
};

#endif //DUOSCRIPT_SCRIPTMETHOD_H
