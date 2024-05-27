// Copyright (c) 2024 Mooshua. All rights reserved.

#include "ScriptVM.h"
#include "ScriptFiber.h"
#include "ScriptCall.h"
#include "ScriptObject.h"
#include "Luau/CodeGen.h"
#include "ScriptHandles.h"
#include "mimalloc.h"
#include "Logging/Log.h"
#include <amtl/am-string.h>


ScriptVM g_ScriptVM;

ScriptVM::ScriptVM()
{
	//	We CANNOT create a Lua_State here because
	//	the constructor is running in the global initialization
	//	context, and Luau isn't set up yet!
}

lua_State* ScriptVM::CreateBox(IScriptRef** box_ref)
{
	lua_State *thread = lua_newthread(L);
	boxes.push_back(thread);

	//	This prevents the thread from writing to the
	//	global state. Note that we still need to
	//	fully lockdown the global state to prevent box-breaking.
	luaL_sandboxthread(thread);

	*box_ref = new ScriptRef(this->L, lua_ref(this->L, -1));
	lua_pop(L, 1);

	return thread;
}

ScriptIsolate::ScriptIsolate(ScriptVM *parent, IIsolateResources* resources)
{
	_parent = parent;
	_resources = resources;

	//	Create the box that this isolate will run in
	L = _parent->CreateBox(&this->thread_reference);

	//	Nothing should ever be running in our root thread...
	//	if something does, then that should be an error.
	//	use fibers for all invocations!
	lua_setthreaddata(L, nullptr);
}

ScriptIsolate::~ScriptIsolate()
{
	//	Destroy all fibers under this isolate.
	for (auto &item : _fibers)
	{
		delete item;
	}

	delete thread_reference;
	lua_close(L);
	L = nullptr;

	//	Set ourselves to nullptr in the VM
	_parent->_isolates[isolate_id] = nullptr;
}

bool ScriptIsolate::TryLoad(const char *name, std::string data, IScriptMethod **method, char *error,
							int maxlength)
{
	int status = luau_load(L, name, data.data(), data.length(), 0);

	if (status == LUA_OK)
	{
		//	If Luau supports JIT-ting on this platform,
		//	then JIT the loaded code. Todo: deeper CodeGen:: API support here
		if (Luau::CodeGen::isSupported()) {
		//	Luau::CodeGen::compile_DEPRECATED(this->L, -1, 0, nullptr);
		}

		//	method on stack, turn into ref.
		int methodRef = lua_ref(L, -1);
		lua_pop(L, 1);

		*method = new ScriptMethod(this->L, methodRef);

		return true;
	}

	//	Error! String on stack.
	const char* err = lua_tostring(L, -1);

	if (error == nullptr || maxlength == 0)
		return false;

	ke::SafeSprintf(error, maxlength, "%s", err);
	return false;
}

IScriptFiber *ScriptIsolate::NewFiber()
{

	lua_State *thread = lua_newthread(this->L);
	IScriptRef* ref = new ScriptRef(this->L, lua_ref(this->L, -1));
	lua_pop(this->L, 1);

	ScriptFiber* fiber = new ScriptFiber(this, thread, ref);
	_fibers.push_back(fiber);

	//	Tell the fiber what their index is in the
	//	_fibers vector, so they can cleanup.
	fiber->fiber_id = _fibers.size() - 1;

	return fiber;
}

ScriptFiber *ScriptIsolate::GetFiber(int id)
{
	return _fibers[id];
}

IsolateHandle *ScriptIsolate::ToHandleInternal()
{
	return new IsolateHandle(isolate_id);
}

IIsolateHandle *ScriptIsolate::ToHandle()
{
	return this->ToHandleInternal();
}

IIsolateResources *ScriptIsolate::GetResources()
{
	return this->_resources;
}

void *ScriptVM::Alloc(void *userdata, void *pointer, size_t oldsize, size_t newsize)
{
	if (newsize == 0)
	{
		mi_free(pointer);
		return nullptr;
	}

	//	Luau allocates pages and does it's own memory management.
	//	mimalloc should only allocate sizes that are good for itself,
	//	because it doesn't need to manage & compact individual objects.
	size_t good_size = mi_good_size(newsize);
	return mi_realloc(pointer, good_size);
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

	fiber->call.SetupCall(argc);

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
			return lua_yield(L, fiber->call.stack_pop);
		case ScriptResult::Clean:
			return fiber->call.stack_pop;
	}
}

IScriptRef *ScriptVM::NewMethod(fastdelegate::FastDelegate<IScriptResult *, IScriptCall *>* callback)
{
	lua_pushlightuserdatatagged(L, callback, 0);
	lua_pushcclosurek(L, &ScriptVM::CFunction, "ScriptVM::CFunction", 1, nullptr);

	int ref = lua_ref(L, -1);
	lua_pop(L, 1);

	return new ScriptRef(L, ref);
}

void ScriptVM::NewGlobal(const char *name, IScriptRef *ref)
{
	lua_getref(L, ref->AsReferenceId());
	lua_setglobal(L, name);
}

void ScriptVM::Initialize()
{
	this->L = lua_newstate(&ScriptVM::Alloc, nullptr);
	this->controllers = new ScriptControllerManager(this->L);

	luaL_openlibs(this->L);

	//	Because people are terrible, i've gone ahead and removed several builtins
	//	to prevent adverse behavior or abuse.
	//	- loadstring/getfenv/setfenv:
	//		Removed due to heavy use in obfuscators
	//		and malicious lua script techniques
	//	- collectgarbage:
	//		Lua 5.1 feature, not in luau, overwriting for the error.
	//	- coroutine.wrap/create:
	//		WILL REPLACE SOON with alternatives that make a fiber.

	//	EXPLICITLY ALLOWED:
	//	- newproxy/xpcall:
	//		VERY useful, especially in userland debuggers.
	//	- debug* library
	//		Already sandbox-safe. Should add stuff to this, to be honest.
	//	- table.freeze:
	//		Any code that errors should be adapted to not error here. Simple as.

	static const luaL_Reg stdlibs[] = {
			{"loadstring", &ScriptVM::DisableStandardLibraryMethod },
			{"getfenv", &ScriptVM::DisableStandardLibraryMethod },
			{"setfenv", &ScriptVM::DisableStandardLibraryMethod },
			{"collectgarbage", &ScriptVM::DisableStandardLibraryMethod },
			{"coroutine.create", &ScriptVM::DisableStandardLibraryMethod },
			{"coroutine.wrap", &ScriptVM::DisableStandardLibraryMethod },
			{"coroutine.running", &ScriptVM::DisableStandardLibraryMethod },

			{"require", &ScriptVM::LuaRequire},
			{ NULL, NULL },
	};

	lua_pushvalue(L, LUA_GLOBALSINDEX);
	luaL_register(L, nullptr, stdlibs);

	g_Log.Component("Luau", Log::STAT_GOOD, "Started");

	if (Luau::CodeGen::isSupported()) {
		Luau::CodeGen::create(L);
		g_Log.Component("Luau JIT", Log::STAT_GOOD, "Started");
	} else {
		g_Log.Component("Luau JIT", Log::STAT_NOTE, "JIT not supported on platform; fallback to interpreter");
	}
}

ScriptIsolate *ScriptVM::GetIsolate(int id)
{
	return _isolates[id];
}

int ScriptVM::DisableStandardLibraryMethod(lua_State *L)
{
	luaL_errorL(L, "This method has been disabled in DuoScript.");
	return 0;
}

IScriptIsolate *ScriptVM::CreateIsolate(IIsolateResources* resources)
{
	return this->CreateIsolateInternal(resources);
}

ScriptIsolate *ScriptVM::CreateIsolateInternal(IIsolateResources* resources)
{
	ScriptIsolate* isolate = new ScriptIsolate(this, resources);
	_isolates.push_back(isolate);

	//	Tell the isolate what their index is in the
	//	_isolates vector, so they can cleanup on destroy.
	isolate->isolate_id = _isolates.size() - 1;

	return isolate;
}

int ScriptVM::LuaRequire(lua_State *L)
{
	std::string originalName = luaL_checkstring(L, -1);
	std::replace( originalName.begin(), originalName.end(), '.', '/');

	std::string name = ke::StringPrintf("%s.luau", originalName.c_str());


	ScriptFiber* fiber = static_cast<ScriptFiber *>(lua_getthreaddata(L));
	ScriptIsolate* isolate = fiber->parent;
	IIsolateResources* resources = isolate->GetResources();

	std::string file;
	if (!resources->TryGetCodeResource(name.c_str(), &file))
		luaL_error(L, "Failed to get resource %s: %s", name.c_str(), file.c_str());

	char error[512];
	IScriptMethod* method;

	if (!isolate->TryLoad(name.c_str(), file, &method, error, sizeof(error)))
		luaL_error(L, "Error loading plugin %s: %s", name.c_str(), error);

	//	Create a fiber to run the loader
	IScriptFiber* newFiber = isolate->NewFiber();
	newFiber->TrySetup(method);
	IScriptReturn* result = newFiber->Call(true);

	IScriptRef* module;
	if (!result->ArgObject(1, &module))
		luaL_error(L, "Module %s must return exactly one value!", name.c_str());

	lua_getref(L, module->AsReferenceId());

	delete newFiber;
	return 1;
}

