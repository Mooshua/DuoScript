// Copyright (c) 2024 Mooshua. All rights reserved.

#include "ScriptFiber.h"
#include "ScriptHandles.h"
#include "ScriptObject.h"

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

	call.Setup(-1);
	call.PushObject(method);

	if (args != nullptr)
		*args = &this->call;

	return true;
}

bool ScriptFiber::TryContinue(IScriptInvoke **args)
{
	if (this->L == nullptr || this->thread_reference == nullptr)
		return false;

	call.Setup(-1);

	if (args != nullptr)
		*args = &this->call;

	return true;
}

IScriptCall *ScriptFiber::Call(bool use)
{
	int status = lua_resume(L, nullptr, call.returnc - this->first_invoke);
	this->first_invoke = false;

	if (status == LUA_OK && !use)
	{
		//	We're fire and forget, so no one is holding on to us.
		//	Delete ourselves so we can be returned to the thread pool.
		delete this;
	}

	if (status != LUA_OK)
	{
		if (status == LUA_YIELD)
		{
			//	TODO
			return nullptr;
		}

		printf("%s\n", lua_tostring(L, -1));
		printf("%i / %s\n", lua_stackdepth(L), lua_debugtrace(L));

		for (int i = 0; i < lua_stackdepth(L); ++i) {
			lua_Debug debug;

			lua_getinfo(L, i, "nsl", &debug);

			printf("\t[%i] %s", i, debug.name);
		}

		printf("======================\n");
	}

	return nullptr;
}
