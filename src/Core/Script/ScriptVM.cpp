// Copyright (c) 2024 Mooshua. All rights reserved.

#include "ScriptVM.h"
#include "ScriptFiber.h"
#include "ScriptCall.h"
#include "ScriptObject.h"
#include "luacodegen.h"
#include "Luau/CodeGen.h"
#include <amtl/am-string.h>

ScriptVM g_ScriptVM;

ScriptVM::ScriptVM()
{
	//	We CANNOT create a Lua_State here because
	//	the constructor is running in the global initialization
	//	context, and Luau isn't set up yet!
}

lua_State* ScriptVM::CreateBox()
{
	lua_State *thread = lua_newthread(L);
	boxes.push_back(thread);

	//	This prevents the thread from writing to the
	//	global state. Note that we still need to
	//	fully lockdown the global state to prevent box-breaking.
	luaL_sandboxthread(thread);

	return thread;
}

ScriptIsolate::ScriptIsolate(ScriptVM *parent)
{
	_parent = parent;
	//	Create the box that this isolate will run in
	L = _parent->CreateBox();

	//	Nothing should ever be running in our root thread...
	//	if something does, then that should be an error.
	//	use fibers for all invocations!
	lua_setthreaddata(L, nullptr);
}

ScriptIsolate::~ScriptIsolate()
{
	lua_close(L);
	L = nullptr;
}

bool ScriptIsolate::TryLoad(const char *name, const char *data, size_t length, IScriptMethod **method, char **error,
							int maxlength)
{
	int status = luau_load(L, name, data, length, 0);

	if (status == LUA_OK)
	{
		//	If Luau supports JIT-ting on this platform,
		//	then JIT the loaded code. Todo: deeper CodeGen:: API support here
		if (Luau::CodeGen::isSupported()) {
			Luau::CodeGen::compile(this->L, -1, 0, nullptr);
		}

		//	method on stack, turn into ref.
		int methodRef = lua_ref(L, -1);
		*method = new ScriptMethod(this->L, methodRef);

		return true;
	}

	//	Error! String on stack.
	const char* err = lua_tostring(L, -1);
	ke::SafeSprintf(*error, maxlength, "%s", err);
	return false;
}

IScriptFiber *ScriptIsolate::NewFiber()
{
	return new ScriptFiber(this);
}

int ScriptVM::CFunction(lua_State *L)
{
	int argc = lua_gettop(L);

	//	Get callback upvalue
	void* callback = lua_tolightuserdata(L, lua_upvalueindex(1));
	fastdelegate::FastDelegate<IScriptResult*, IScriptCall*>* delegate
		= static_cast<fastdelegate::FastDelegate<IScriptResult*, IScriptCall*>*>(callback);

	//	We call delegate.clear() on controller unload
	if (delegate->empty())
		luaL_errorL(L, "Attempt to use native from unloaded module");

	//	Get running fiber
	ScriptFiber* fiber = static_cast<ScriptFiber *>(lua_getthreaddata(L));
	if (fiber == nullptr)
		luaL_errorL(L, "Invoking ScriptVM::CFunction in a non-fiber thread!");

	fiber->call.Setup(argc);

	ScriptResult* result = static_cast<ScriptResult *>((*delegate)(&fiber->call));

	if (result == nullptr)
		luaL_error(L, "Null ScriptResult returned from native code");

	switch (result->strategy)
	{
		default:
		case ScriptResult::Invalid:
			luaL_error(L, "Invalid ScriptResult returned from native code");
		case ScriptResult::Error:
			lua_error(L);
		case ScriptResult::Yield:
			return lua_yield(L, fiber->call.returnc);
		case ScriptResult::Clean:
			return fiber->call.returnc;
	}
}

IScriptRef *ScriptVM::NewMethod(fastdelegate::FastDelegate<IScriptResult *, IScriptCall *>* callback)
{
	lua_pushlightuserdatatagged(L, callback, 0);
	lua_pushcclosurek(L, &ScriptVM::CFunction, "ScriptVM::CFunction", 1, nullptr);

	int ref = lua_ref(L, -1);
	return new ScriptRef(L, ref);
}

void ScriptVM::NewGlobal(const char *name, IScriptRef *ref)
{
	lua_getref(L, ref->AsReferenceId());
	lua_setglobal(L, name);
}

IScriptIsolate *ScriptVM::CreateIsolate()
{
	return new ScriptIsolate(this);
}

void ScriptVM::Initialize()
{
	this->L = luaL_newstate();
	this->controllers = new ScriptControllerManager(this->L);

	luaL_openlibs(this->L);

	if (Luau::CodeGen::isSupported()) {
		Luau::CodeGen::create(L);
	}
}

