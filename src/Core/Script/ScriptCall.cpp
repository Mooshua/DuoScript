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
			return false;                                \
		if (result == nullptr) 						\
			return true;                                        \
		*result = get ( this->parent->L , slot); \
		return true; \
	}

#define CREATE_PUSHER(name, type, push) \
	void name (type value)                 \
    {                                   \
		lua_checkstack(this->parent->L, 1); \
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

	if (result == nullptr)
		return true;

	int ref = lua_ref( this->parent->L , slot);
	*result = new ScriptRef(this->parent->L, ref);

	return true;
}

void ScriptCall::PushObject(IScriptRef *value)
{
	lua_checkstack(this->parent->L, 1);

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

	if (result == nullptr)
		return true;

	size_t length;
	const char* str = lua_tolstring(this->parent->L, slot, &length);

	if (length == 0 || str == nullptr)
		return false;

	if (written != nullptr)
		*written = std::min( (size_t)maxlen, length);

	strcpy_s(result, maxlen, str);
	return true;
}

void ScriptCall::PushString(const char *value, int length)
{
	lua_checkstack(this->parent->L, 1);

	if (length == -1)
		length = strlen(value);

	returnc++;
	lua_pushlstring(this->parent->L, value, length);
}


IIsolateHandle *ScriptCall::GetIsolate()
{
	return this->parent->parent->ToHandle();
}

IFiberHandle *ScriptCall::GetFiber()
{
	return this->parent->ToHandle();
}


IScriptResult *ScriptCall::Error(const char *error, ...)
{
	va_list args;
	va_start(args, error);

	//	Push error string onto stack
	lua_pushvfstring(this->parent->L, error, args);
	//	lua_pushstring(this->parent->L, error);
	result.strategy = ScriptResult::Error;

	va_end(args);

	return &result;
}

IScriptResult *ScriptCall::Await()
{
	result.strategy = ScriptResult::Yield;

	return &result;
}

IScriptResult *ScriptCall::Return()
{
	result.strategy = ScriptResult::Clean;

	return &result;
}

bool ScriptCall::ArgMethod(int slot, IScriptMethod **result)
{
	if (slot > argc)
		return false;

	slot++;

	if (!lua_isfunction( this->parent->L , slot ))
		return false;

	if (result == nullptr)
		return true;

	int ref = lua_ref( this->parent->L , slot);
	*result = new ScriptMethod(this->parent->L, ref);

	return true;
}

bool ScriptCall::ArgTable(int slot, IScriptObject **result)
{
	if (slot > argc)
		return false;

	slot++;

	if (!lua_istable( this->parent->L , slot ))
		return false;

	if (result == nullptr)
		return true;

	int ref = lua_ref( this->parent->L , slot);
	*result = new ScriptObject(this->parent->parent->L, ref);

	return true;
}


bool ScriptCall::ArgBuffer(int slot, void **result, size_t *size)
{
	if (slot > argc)
		return false;

	slot++;

	if (!lua_isbuffer( this->parent->L , slot ))
		return false;

	if (result == nullptr)
		return true;


	*result = lua_tobuffer(this->parent->L, slot, size);

	return true;
}

void ScriptCall::PushVarArgs(IScriptCall *from, int after)
{
	ScriptCall* friend_from = static_cast<ScriptCall *>(from);
	lua_checkstack(this->parent->L, from->GetLength());

	for (int arg = after; arg < from->GetLength(); arg++)
	{
		//	Use xpush to toss across lua_states
		lua_xpush(friend_from->parent->L, this->parent->L, arg + 1);
		returnc++;

		/*
		IScriptRef* ref;
		if (!from->ArgObject(arg, &ref))
			continue;

		this->PushObject(ref);
		delete ref;*/

	}
}

void ScriptCall::PushArg(IScriptCall *from, int arg)
{
	ScriptCall* friend_from = static_cast<ScriptCall *>(from);
	lua_checkstack(this->parent->L, 1);

	//	Erm... we don't have an arg here guys!
	//	if (arg > from->GetLength())
	//		return;

	lua_xpush(friend_from->parent->L, this->parent->L, arg + 1);
	returnc++;
}

void ScriptCall::PushBuffer(void *data, size_t length)
{
	lua_checkstack(this->parent->L, 1);
	void* buffer = lua_newbuffer(this->parent->L, length);
	memcpy(buffer, data, length);

	returnc++;
}

