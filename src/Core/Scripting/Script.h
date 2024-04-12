// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_SCRIPT_H
#define DUOSCRIPT_SCRIPT_H

#include "lua.h"
#include "lualib.h"

#include "DuoMetamod.h"

#include "IScript.h"
#include "IScriptController.h"


class ScriptCall : public IScriptCall
{
public:
	ScriptCall(lua_State* L)
		: L(L)
		, argc(lua_gettop(L))
	{}

public:
	virtual IScriptIsolate *GetIsolate() final;

	virtual void* GetOpaqueSelf() final;

	virtual int GetLength() final;

	///	Generate a response for this call that invokes an error
	///	in the calling method.
	virtual IScriptResult* Error(const char* error) final;

	///	Suspend execution until the provided task returns.
	virtual IScriptResult* Await() final;
protected:
	lua_State *L;
	int argc;
};

class ScriptNamecall : public ScriptCall, public IScriptNamecall
{
public:
	explicit ScriptNamecall(lua_State* L)
		: ScriptCall(L)
	{}
public:
	virtual const char* GetNamecall() override;
	virtual IScriptCall* GetArgs() override;
};


#endif //DUOSCRIPT_SCRIPT_H
