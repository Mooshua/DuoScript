// Copyright (c) 2024 Mooshua. All rights reserved.

#include "ScriptMethod.h"

void ScriptCaller::PushNumber(double number)
{
	lua_pushnumber(L, number);
	arg_count++;
}

void ScriptCaller::PushRef(IScriptRef *ref)
{
	lua_getref(L, ref->AsReferenceId());
	arg_count++;
}

void ScriptCaller::PushObject(IScriptObject *object)
{
	return this->PushRef(object);
}

bool ScriptCaller::TryExecute(IScriptError **error)
{
	int result = lua_resume(L, nullptr, arg_count);

	if (result == LUA_YIELD)
		return true;

	if (result == LUA_OK)
		return true;

	//	TODO: Argument parsing

	return false;
}

void ScriptCaller::Abandon()
{
	lua_pop(L, arg_count);
	arg_count = 0;
}

int ScriptMethod::AsReferenceId()
{
	return ref;
}

IScriptExecution *ScriptMethod::Invoke()
{
	lua_State* coroutine = lua_newthread(L);
	//	Inherit userdata pointer from parent
	lua_setthreaddata(coroutine, lua_getthreaddata(L));

	return new ScriptExecution(coroutine, this);
}

bool ScriptExecution::IsResumable()
{
	int status = lua_costatus(nullptr, L);

	switch (status)
	{
		case LUA_CONOR:
		case LUA_COSUS:
			return true;
		default:
			return false;
	}
}

IScriptCaller *ScriptExecution::Resume()
{
	if (!this->IsResumable())
		return nullptr;

	//	Stack should already be set up for call in this state,
	//	assuming no one's pulling our leg.

	return new ScriptCaller(L);
}
