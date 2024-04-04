// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_SCRIPTVM_H
#define DUOSCRIPT_SCRIPTVM_H

#define COMPILING_DUOSCRIPT
#include "Macro.h"

#include <lua.h>
#include <lualib.h>

#include <IScript.h>
#include <vector>
#include "IScriptMethod.h"
#include "ScriptControllers.h"

///	A virtual machine manages the global environment,
///	but does not actually run code directly in that environment.
///	The VM spawns "Isolates" which can then be used to run code in
///	sandboxed environments.
class ScriptVM : IScript
{
public:
	ScriptVM();

	void Initialize() override;

	///	Create a new boxed thread that can be
	///	used to run code in a pseudosandbox.
	lua_State* CreateBox();

	IScriptIsolate* CreateIsolate() override;

	///	Get the underlying lua state for low-level
	///	operations on this VM
	operator lua_State*() { return L; }

	///	Create a new method that invokes the specified delegate
	IScriptRef *NewMethod(fastdelegate::FastDelegate<IScriptResult *, IScriptCall *> *callback);

	///	Make a IScriptRef* value a global
	void NewGlobal(const char* name, IScriptRef* ref);

protected:
	static int CFunction(lua_State *L);

protected:
	std::vector<lua_State *> boxes;

public:
	ScriptControllerManager* controllers;
	lua_State *L;

};

DUO_EXPORT ScriptVM g_ScriptVM;

class ScriptIsolate : public IScriptIsolate
{
public:
	explicit ScriptIsolate(ScriptVM* parent);

	~ScriptIsolate();

	///	Attempt to load compiled luau bytecode into the virtual machine.
	///	Returns true and writes to *method on success,
	///	false and writes to *error on failure.
	virtual bool TryLoad(const char* name, const char* data, size_t length, IScriptMethod **method, char ** error, int maxlength);

	IScriptFiber* NewFiber() final;

public:
	lua_State *L;
protected:
	ScriptVM* _parent;
};

#endif //DUOSCRIPT_SCRIPTVM_H
