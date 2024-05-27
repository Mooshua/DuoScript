// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "ScriptObject.h"

ScriptRef::ScriptRef(lua_State *L, int ref)
{
	this->L = L;
	this->ref = ref;
}

ScriptRef::~ScriptRef()
{
	lua_unref(L, ref);
	this->L = nullptr;
	this->ref = -1;
}

int ScriptRef::AsReferenceId()
{
	return this->ref;
}

IScriptRef *ScriptRef::Clone()
{
	lua_getref(L, this->ref);
	int newref = lua_ref(L, -1);
	lua_pop(L, 1);

	return new ScriptRef(this->L, newref);
}


#define SCRIPT_ACCESSOR(name, index, indexor, result, resultor) \
	bool ScriptObject:: name (index i, result r)                       \
    { \
        this-> indexor (i);                                           \
        bool success = this-> resultor (r);                            \
        this->IndexPop();                                             \
        return success;                                         \
	}

#define SCRIPT_INDEXORS(name, result, resultor) \
	SCRIPT_ACCESSOR(name, int, IndexInt, result, resultor) \
	SCRIPT_ACCESSOR(name, const char*, IndexKey, result, resultor) \


SCRIPT_INDEXORS(TryGetNumber, double*, TryToNumber);
SCRIPT_INDEXORS(TryGetString, std::string*, TryToString);
SCRIPT_INDEXORS(TryGetObject, IScriptObject**, TryToObject);

bool ScriptObject::IndexKey(const char *string)
{
	lua_getref(L, ref);
	lua_rawgetfield(L, -1, string);

	if (lua_type(L, -1) == LUA_TNIL)
	{
		lua_pop(L, 2);
		return false;
	}

	return true;
}

bool ScriptObject::IndexInt(int index)
{
	lua_getref(L, ref);
	lua_rawgeti(L, -1, index);

	if (lua_type(L, -1) == LUA_TNIL)
	{
		lua_pop(L, 2);
		return false;
	}

	return true;
}


void ScriptObject::IndexPop()
{
	lua_pop(L, 2);
}

bool ScriptObject::TryToNumber(double *result)
{
	//	Todo: Maybe add .tonumber call here if not a number?
	if (lua_type(L, -1) != LUA_TNUMBER)
		return false;

	*result = lua_tonumber(L, -1);

	return true;
}

bool ScriptObject::TryToString(std::string *result)
{
	if (lua_type(L, -1) != LUA_TSTRING)
		return false;

	const char *internal_string = lua_tostring(L, -1);
	*result = *internal_string;

	return true;
}

bool ScriptObject::TryToObject(IScriptObject **result)
{
	if (lua_type(L, -1) != LUA_TTABLE)
		return false;

	int reference = lua_ref(L, -1);
	*result = new ScriptObject(L, reference);

	return true;
}

ScriptObject::ScriptObject(lua_State *L, int i) : ScriptRef(L, i)
{

}
