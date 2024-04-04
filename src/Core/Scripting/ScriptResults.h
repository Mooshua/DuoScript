// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_SCRIPTRESULTS_H
#define DUOSCRIPT_SCRIPTRESULTS_H

#include <lua.h>
#include <lualib.h>

#include "IScript.h"

class YieldScriptResult : public IScriptResult
{
public:
	virtual int Execute() final;

	YieldScriptResult(lua_State *L, ICallback* callback)
	: L(L)
	, callback(callback)
	{}

protected:
	lua_State *L;
	ICallback *callback;
};

class ErrorScriptResult : public IScriptResult
{
public:
	virtual int Execute() final;

	ErrorScriptResult(lua_State *L, const char* error)
		: L(L)
		, err(error)
	{}

protected:
	lua_State *L;
	const char* err;
};



#endif //DUOSCRIPT_SCRIPTRESULTS_H
