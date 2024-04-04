// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_SCRIPTISOLATE_H
#define DUOSCRIPT_SCRIPTISOLATE_H


#include "ScriptVM.h"
#include "lua.h"
#include "lualib.h"
#include "IScriptMethod.h"

class ScriptIsolate
{
public:
	ScriptIsolate(ScriptVM* vm)
		: _vm(vm)
	{
		_thread = lua_newthread(vm->L);
		luaL_sandboxthread(_thread);
		lua_setthreaddata(_thread, this);
	}

public:
	virtual bool TryLoad(const char* name, const char* data, size_t size, IScriptMethod **method);

	///	Determine if we can yield on the current isolate.
	///	When false, we should throw an error if we attempt to yield.
	///	Used on the main thread eg. in hooks to prevent undefined behavior
	virtual bool IsYieldable();

	virtual void SetYieldable(bool yieldable);
private:
	bool is_yieldable;

	ScriptVM* _vm;

	///	The lua_state for the thread
	///	used for this isolate.
	lua_State* _thread;
};


#endif //DUOSCRIPT_SCRIPTISOLATE_H
