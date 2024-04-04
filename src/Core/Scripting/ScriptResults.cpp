// Copyright (c) 2024 Mooshua. All rights reserved.

#include "ScriptResults.h"
#include "ScriptIsolate.h"

int ErrorScriptResult::Execute()
{
	lua_pushstring(L, err);
	lua_error(L);
}

int YieldScriptResult::Execute()
{
	ScriptIsolate* isolate = static_cast<ScriptIsolate *>(lua_getthreaddata(L));

	if (!isolate->IsYieldable())
		luaL_errorL(L, "Attempt to yield in unyieldable isolate!");

	//	TODO: ADD CALLBACK LISTENER HERE
	lua_yield(L, 0);
}
