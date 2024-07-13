// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "ScriptFiber.h"
#include "ScriptCall.h"
#include "ScriptObject.h"

#include <amtl/am-platform.h>
#include <utility>

ScriptCall::ScriptCall(ScriptFiber* parent)
{
	this->parent = parent;
	this->stack_push = -1;
	this->stack_pop = -1;
}

void *ScriptCall::GetOpaqueSelf()
{
	if (1 > stack_push)
		return false;

	if (!lua_isuserdata(this->parent->L, 1))
		return false;

	return lua_touserdata(this->parent->L, 1);
}

void ScriptCall::SetupCall(int argc)
{
	this->has_self = true;
	this->stack_push = argc;
	this->result.strategy = ScriptResult::Invalid;
	this->stack_pop = 0;
}


void ScriptCall::SetupReturn(int argc, int status)
{
	this->has_self = false;
	this->last_status = status;

	this->stack_push = argc;
	this->result.strategy = ScriptResult::Invalid;
	this->stack_pop = 0;
}

int ScriptCall::GetLength()
{
	return this->stack_push;
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
		if (slot > stack_push)						   \
			return false;                                \
        slot += this->has_self; /* Remove self pointer */  \
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
		stack_pop++;                          \
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
	if (slot > stack_push)
		return false;

	slot += this->has_self;

	if (result == nullptr)
		return true;

	int ref = lua_ref( this->parent->L , slot);
	*result = new ScriptRef(this->parent->L, ref);

	return true;
}

void ScriptCall::PushObject(IScriptRef *value)
{
	lua_checkstack(this->parent->L, 1);

	stack_pop++;
	lua_getref(this->parent->L, value->AsReferenceId());
}

bool ScriptCall::ArgString(int slot, char* result, int maxlen, int* written)
{
	if (slot > stack_push)
		return false;

	slot += this->has_self;

	if (!lua_isstring( this->parent->L , slot ))
		return false;

	if (result == nullptr)
		return true;

	size_t length;
	const char* str = lua_tolstring(this->parent->L, slot, &length);

	if (length == 0 || str == nullptr || maxlen <= length)
		return false;

	if (written != nullptr)
		*written = std::min( (size_t)maxlen, length);

	strcpy_s(result, maxlen, str);
	return true;
}

bool ScriptCall::ArgString(int slot, std::string *result)
{
	if (slot > stack_push)
		return false;

	slot += this->has_self;

	if (!lua_isstring( this->parent->L , slot ))
		return false;

	size_t length;
	const char* str = lua_tolstring(this->parent->L, slot, &length);

	result->append(str, length);
	return true;
}


void ScriptCall::PushString(const char *value, int length)
{
	lua_checkstack(this->parent->L, 1);

	if (length == -1)
		length = strlen(value);

	stack_pop++;
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
	//	When debugging, it can be difficult to tell where
	//	an error comes from if this is not breakpointed.
	//	Place a breakpoint somewhere in here to see where
	//	the error is actually called, and not to get
	//	the result from the FastDelegate invocation.

	va_list args;
	va_start(args, error);
	{
		//	Push error string onto stack
		lua_pushvfstring(this->parent->L, error, args);
		//	lua_pushstring(this->parent->L, error);
		result.strategy = ScriptResult::Error;
	}
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
	if (slot > stack_push)
		return false;

	slot += this->has_self;

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
	if (slot > stack_push)
		return false;

	slot += this->has_self;

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
	if (slot > stack_push)
		return false;

	slot += this->has_self;

	if (!lua_isbuffer( this->parent->L , slot ))
		return false;

	if (result == nullptr)
		return true;


	*result = lua_tobuffer(this->parent->L, slot, size);

	return true;
}

bool ScriptCall::ArgBuffer(int slot, std::string *result)
{
	if (slot > stack_push)
		return false;

	slot += this->has_self;

	if (!lua_isbuffer( this->parent->L , slot ))
		return false;

	size_t size;
	char* buffer = static_cast<char *>(lua_tobuffer(this->parent->L, slot, &size));

	result->append(buffer, size);
	return true;
}

bool ScriptCall::ArgOpaqueEntity(int slot, IBaseScriptControllerEntity **userdata)
{
	if (slot > stack_push)
		return false;

	slot += this->has_self;

	if (!lua_isuserdata(this->parent->L, slot))
		return false;

	void *udata = lua_touserdata(this->parent->L, 1);
	*userdata = static_cast<IBaseScriptControllerEntity*>(udata);

	return true;
}

void ScriptCall::PushVarArgs(IScriptPolyglotView *from, int after)
{
	ScriptCall* friend_from = static_cast<ScriptCall *>( from->ToPolyglot() );
	lua_checkstack(this->parent->L, friend_from->GetLength());

	for (int arg = after; arg < friend_from->GetLength(); arg++)
	{
		//	Use xpush to toss across lua_states
		lua_xpush(friend_from->parent->L, this->parent->L, arg + friend_from->has_self);
		stack_pop++;

		/*
		IScriptRef* ref;
		if (!from->ArgObject(arg, &ref))
			continue;

		this->PushObject(ref);
		delete ref;*/

	}
}

void ScriptCall::PushArg(IScriptPolyglotView *from, int arg)
{
	ScriptCall* friend_from = static_cast<ScriptCall *>( from->ToPolyglot() );
	lua_checkstack(this->parent->L, 1);

	//	Erm... we don't have an arg here guys!
	//	if (arg > from->GetLength())
	//		return;

	lua_xpush(friend_from->parent->L, this->parent->L, arg + friend_from->has_self);
	stack_pop++;
}

void ScriptCall::PushBuffer(void *data, size_t length)
{
	lua_checkstack(this->parent->L, 1);
	void* buffer = lua_newbuffer(this->parent->L, length);
	memcpy(buffer, data, length);

	stack_pop++;
}

bool ScriptCall::IsYielding()
{
	return last_status == LUA_YIELD;
}

bool ScriptCall::IsError()
{
	//	if statements can suck it
	switch (last_status)
	{
		case LUA_ERRRUN:
			//	These two should never happen.
		case LUA_ERRMEM:
		case LUA_ERRERR:
			return true;
		default:
			return false;
	}
}

const char *ScriptCall::GetError()
{
	if (stack_push != 1)
		return "(no error. this is bad. you should not see this. go yell at someone.)";

	//	Okay. IN THE EVENT that the object error()'d
	//	is not a string but a malformed object,
	//	you all can just crash or whatever. I don't even care.
	//	TODO: Start caring when we get around to sandboxing
	return lua_tostring(this->parent->L, -1);
}

IScriptPolyglot *ScriptCall::ToPolyglot()
{
	return this;
}




