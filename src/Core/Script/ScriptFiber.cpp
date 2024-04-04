// Copyright (c) 2024 Mooshua. All rights reserved.

#include "ScriptFiber.h"

ScriptFiber::ScriptFiber(ScriptIsolate *parent)
	: call(ScriptCall(this))
{
	this->parent = parent;
	this->L = lua_newthread(parent->L);
	lua_setthreaddata(L, this);
}

bool ScriptFiber::IsReady()
{
	if (!lua_isthreadreset(L))
		return false;


	return true;
}

bool ScriptFiber::TrySetup(IScriptMethod *method)
{
	if (!this->IsReady())
		return false;

	call.Setup(-1);
	lua_getref(L, method->AsReferenceId());

	return true;
}

IScriptCall *ScriptFiber::Call()
{
	int status = lua_resume(L, nullptr, call.returnc);

	if (status != LUA_OK)
	{
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
