// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_SCRIPTVM_H
#define DUOSCRIPT_SCRIPTVM_H


#include "lua.h"
#include "lualib.h"

#include "IScript.h"

class ScriptVM : IScript
{
public:
	ScriptVM()
	{
		L = luaL_newstate();
		luaL_sandbox(L);

		lua_Callbacks *callbacks = lua_callbacks(L);

		callbacks->interrupt = &ScriptVM::InterruptHandler;
	}

public:
	lua_State *L;

protected:
	static void InterruptHandler(lua_State *L, int gc_state);
};

extern ScriptVM g_ScriptVM;

#endif //DUOSCRIPT_SCRIPTVM_H
