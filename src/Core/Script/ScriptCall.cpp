// Copyright (c) 2024 Mooshua. All rights reserved.

#include <utility>
#include "ScriptFiber.h"
#include "ScriptCall.h"
#include "ScriptObject.h"

ScriptCall::ScriptCall(ScriptFiber* parent)
{
	this->parent = parent;
	this->argc = -1;
	this->returnc = -1;
}

void *ScriptCall::GetOpaqueSelf()
{
	if (1 > argc)
		return false;

	if (!lua_isuserdata(this->parent->L, 1))
		return false;

	return lua_touserdata(this->parent->L, 1);
}

void ScriptCall::Setup(int argc)
{
	this->argc = argc;
	this->result.strategy = ScriptResult::Invalid;
	this->returnc = 0;
}

int ScriptCall::GetLength()
{
	return this->argc;
}

bool ScriptCall::TryGetNamecall(char *result, int maxlen)
{
	const char* namecall = lua_namecallatom(this->parent->L, nullptr);

	//	No namecall?
	if (namecall == nullptr)
		return false;

	strcpy_s(result, maxlen, namecall);
	return true;
}

#define CREATE_ACCESSOR(name, type, check, get) \
	bool name (int slot, type *result )            \
	{                                              \
		if (slot > argc)						   \
			return false;                                \
        slot++; /* Remove self pointer */                  \
		if (! check ( this->parent->L , slot )) \
			return false; \
		*result = get ( this->parent->L , slot); \
		return true; \
	}

#define CREATE_PUSHER(name, type, push) \
	void name (type value)                 \
    {                                   \
    	returnc++;                          \
		push (this->parent->L, value);\
	}                                    \
                                        \

CREATE_ACCESSOR(ScriptCall::ArgBool, bool, lua_isboolean , lua_toboolean );
CREATE_PUSHER(ScriptCall::PushBool, bool, lua_pushboolean );

CREATE_ACCESSOR(ScriptCall::ArgNumber, double, lua_isnumber , lua_tonumber );
CREATE_PUSHER(ScriptCall::PushNumber, double, lua_pushnumber );

CREATE_ACCESSOR(ScriptCall::ArgInt, int, lua_isnumber , lua_tointeger );
CREATE_PUSHER(ScriptCall::PushInt, int, lua_pushinteger );

CREATE_ACCESSOR(ScriptCall::ArgUnsigned, unsigned , lua_isnumber , lua_tounsigned );
CREATE_PUSHER(ScriptCall::PushUnsigned, unsigned, lua_pushunsigned );

bool ScriptCall::ArgObject(int slot, IScriptRef** result)
{
	if (slot > argc)
		return false;

	slot++;

	if (!lua_istable( this->parent->L , slot ))
		return false;

	int ref = lua_ref( this->parent->L , slot);
	*result = new ScriptRef(this->parent->L, ref);

	return true;
}

void ScriptCall::PushObject(IScriptRef *value)
{
	returnc++;
	lua_getref(this->parent->L, value->AsReferenceId());
}

bool ScriptCall::ArgString(int slot, char* result, int maxlen, int* written)
{
	if (slot > argc)
		return false;

	slot++;

	if (!lua_isstring( this->parent->L , slot ))
		return false;

	size_t length;
	const char* str = lua_tolstring(this->parent->L, slot, &length);

	if (length == 0 || str == nullptr)
		return false;

	if (written != nullptr)
		*written = std::min( (size_t)maxlen, length);

	strcpy_s(result, std::min( (size_t)maxlen, length), str);
	return true;
}

void ScriptCall::PushString(const char *value, int length)
{
	if (length == -1)
		length = strlen(value);

	returnc++;
	lua_pushlstring(this->parent->L, value, length);
}


IScriptIsolate *ScriptCall::GetIsolate()
{
	return this->parent->parent;
}

IScriptResult *ScriptCall::Error(const char *error)
{
	//	Push error string onto stack
	lua_pushstring(this->parent->L, error);
	result.strategy = ScriptResult::Error;

	return &result;
}

IScriptResult *ScriptCall::Yield()
{
	result.strategy = ScriptResult::Yield;

	return &result;
}

IScriptResult *ScriptCall::Return()
{
	result.strategy = ScriptResult::Clean;

	return &result;
}


