// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "ScriptFiber.h"
#include "ScriptHandles.h"
#include "ScriptObject.h"
#include "Logging/Log.h"

#include <amtl/am-string.h>

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
		this->OnError(lua_tostring(L, -1));
	}

	//	Resume any fibers waiting on the results of this fiber
	if (status != LUA_YIELD) {
		for (IFiberHandle *dependant: _dependants) {
			if (!dependant->Exists())
				continue;

			IScriptFiber *fiber = dependant->Get();
			IScriptInvoke *invoke;
			if (fiber->TryContinue(&invoke)) {
				//	First return: Was the fiber successful?
				invoke->PushBool(!call.IsError());
				invoke->PushVarArgs(call.ToPolyglot());

				fiber->Call(false);
			}
		}
	}

	if (!use && status != LUA_YIELD) {
		//	We're fire and forget, so no one is holding on to us.
		//	Delete ourselves so we can be returned to the thread pool.
		delete this;
		return nullptr;
	}

	return &call;
}

void ScriptFiber::Kill(const char *fmt, ...)
{
	char buffer[2048];

	va_list args;
	va_start(args, fmt);
		ke::SafeVsprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

	this->OnError(buffer);
}

void ScriptFiber::OnError(const char *error)
{
	g_Log.Message(this->parent->GetResources()->GetName(), ILogger::SEV_ERROR,
				  "Script error: %s", error);
	lua_Debug debug;

	for (int i = 0; i < lua_stackdepth(L); ++i) {

		lua_getinfo(L, i, "nsl", &debug);

		g_Log.Blank("    [%s:%i] %s @ %s",
					  debug.short_src, debug.linedefined, debug.what, debug.name);
	}

	//	Reset this thread since we've errored.
	//	This prevents it from ever being used, but doesn't actually free it
	//	(so we can still safely hold on to this fiber for a bit)
	lua_resetthread(this->L);
}

bool ScriptFiber::TryDepend(IScriptFiber *other)
{
	_dependants.push_back(other->ToHandle());
	return true;
}
