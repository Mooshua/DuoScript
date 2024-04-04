// Copyright (c) 2024 Mooshua. All rights reserved.

#include "ScriptIsolate.h"
#include "IScriptMethod.h"


bool ScriptIsolate::TryLoad(const char *name, const char *data, size_t size, IScriptMethod **method)
{
	if (!luau_load(_thread, name, data, size, 0))
		return false;

	//	Make a reference to the created prototype
	lua_ref(_thread, -1);

	return true;
}

bool ScriptIsolate::IsYieldable()
{
	return is_yieldable;
}

void ScriptIsolate::SetYieldable(bool yieldable)
{
	is_yieldable = yieldable;
}
