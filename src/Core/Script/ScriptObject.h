// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#ifndef DUOSCRIPT_SCRIPTOBJECT_H
#define DUOSCRIPT_SCRIPTOBJECT_H

#include <IScript.h>
#include "lua.h"

class ScriptRef : public virtual IScriptRef
{
public:
	ScriptRef(lua_State *L, int ref);
	~ScriptRef() override;


	int AsReferenceId() final;
	IScriptRef* Clone() final;
public:
	lua_State *L;
	int ref;
};

class ScriptMethod : public ScriptRef, public virtual IScriptMethod
{
public:
	ScriptMethod(lua_State *L, int ref)
		: ScriptRef(L, ref)
	{

	}
};

class ScriptObject : public ScriptRef, public virtual IScriptObject
{
public:
	ScriptObject(lua_State *L, int i);

	bool TryGetNumber(const char* key, double *result) override;
	bool TryGetNumber(int index      , double *result) override;

	bool TryGetString(const char* key, std::string *result) override;
	bool TryGetString(int index      , std::string *result) override;

	bool TryGetObject(const char* key, IScriptObject **result) override;
	bool TryGetObject(int index      , IScriptObject **result) override;

protected:
	///	Cleanup the stack used by IndexKey and IndexInt
	///	should be called before every exit
	void IndexPop();
	bool IndexKey(const char* key);
	bool IndexInt(int index);

	bool TryToNumber(double *result);
	bool TryToString(std::string *result);
	bool TryToObject(IScriptObject **result);

};


#endif //DUOSCRIPT_SCRIPTOBJECT_H
