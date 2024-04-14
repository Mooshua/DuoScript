// Copyright (c) 2024 Mooshua. All rights reserved.

#include "ScriptFiber.h"
#include "ScriptHandles.h"
#include "ScriptObject.h"
#include "Logging/Log.h"

ScriptFiber::ScriptFiber(ScriptIsolate *parent, lua_State *L, IScriptRef* thread_ref)
	: call(ScriptCall(this))
{
	//	lua_checkstack(parent->L, 1);

	this->first_invoke = true;
	this->parent = parent;
	//this->L = lua_newthread(parent->L);

	this->L = L;
	lua_setthreaddata(L, this);

	//this->thread_reference = new ScriptRef(this->parent->L, lua_ref(this->parent->L, -1));
	//lua_pop(this->parent->L, 1);
	this->thread_reference = thread_ref;
}

ScriptFiber::~ScriptFiber()
{
	delete this->thread_reference;

	lua_resetthread(this->L);
	this->L = nullptr;
}

bool ScriptFiber::IsReady()
{
	if (this->L == nullptr || this->thread_reference == nullptr)
		return false;

	if (!lua_isthreadreset(L))
		return false;

	//if (lua_costatus(nullptr, L) == LUA_COFIN)
	//	return false;

	return true;
}

IFiberHandle *ScriptFiber::ToHandle()
{
	return new FiberHandle(parent->ToHandleInternal(), fiber_id);
}

bool ScriptFiber::TrySetup(IScriptMethod *method, IScriptInvoke** args)
{
	if (!this->IsReady())
		return false;

	call.SetupCall(-1);
	call.PushObject(method);

	if (args != nullptr)
		*args = &this->call;

	return true;
}

bool ScriptFiber::TryContinue(IScriptInvoke **args)
{
	if (this->L == nullptr || this->thread_reference == nullptr)
		return false;

	call.SetupCall(-1);

	if (args != nullptr)
		*args = &this->call;

	return true;
}

IScriptReturn *ScriptFiber::Call(bool use)
{
	int status = lua_resume(L, nullptr, call.stack_pop - this->first_invoke);
	this->first_invoke = false;

	call.SetupReturn(lua_gettop(L), status);

	if (call.IsError()) {

		g_Log.Message("Luau", ILogger::SEV_ERROR,
					  "Script error: %s", lua_tostring(L, -1));
		lua_Debug debug;

		for (int i = 0; i < lua_stackdepth(L); ++i) {

			lua_getinfo(L, i, "nsl", &debug);

			g_Log.Message("Luau", ILogger::SEV_ERROR,
						  "    [%s:%i] %s @ %s",
						  debug.short_src, debug.linedefined, debug.what, debug.name);
		}
	}

	if (!use)
	{
		//	We're fire and forget, so no one is holding on to us.
		//	Delete ourselves so we can be returned to the thread pool.
		delete this;
		return nullptr;
	}

	return &call;
}
