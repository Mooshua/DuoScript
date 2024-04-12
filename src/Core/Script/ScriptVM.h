// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_SCRIPTVM_H
#define DUOSCRIPT_SCRIPTVM_H

#include "Macro.h"

#include <lua.h>
#include <lualib.h>

#include <IScript.h>
#include <vector>
#include "IScriptMethod.h"
#include "ScriptControllers.h"
#include "ScriptFiber.h"
#include "ScriptHandles.h"

class ScriptIsolate;
class IsolateHandle;

///	A virtual machine manages the global environment,
///	but does not actually run code directly in that environment.
///	The VM spawns "Isolates" which can then be used to run code in
///	sandboxed environments.
class ScriptVM : IScript
{
	friend class ScriptIsolate;
public:
	ScriptVM();

	void Initialize() override;

	///	Create a new boxed thread that can be
	///	used to run code in a pseudosandbox.
	lua_State* CreateBox(IScriptRef** box_ref);

	IScriptIsolate* CreateIsolate() override;
	ScriptIsolate* CreateIsolateInternal();

	ScriptIsolate* GetIsolate(int id);

	///	Get the underlying lua state for low-level
	///	operations on this VM
	operator lua_State*() { return L; }

	///	Create a new method that invokes the specified delegate
	IScriptRef *NewMethod(fastdelegate::FastDelegate<IScriptResult *, IScriptCall *> *callback);

	///	Make a IScriptRef* value a global
	void NewGlobal(const char* name, IScriptRef* ref);

protected:
	static int DisableStandardLibraryMethod(lua_State *L);
	static int CFunction(lua_State *L);
	static void* Alloc(void* userdata, void* pointer, size_t oldside, size_t newsize);

protected:
	std::vector<lua_State *> boxes;
	std::vector<ScriptIsolate*> _isolates;

public:
	ScriptControllerManager* controllers;
	lua_State *L;

};

extern ScriptVM g_ScriptVM;

class ScriptIsolate : public IScriptIsolate
{
	friend class ScriptVM;
	friend class ScriptFiber;
public:
	explicit ScriptIsolate(ScriptVM* parent);

	~ScriptIsolate();

	IsolateHandle* ToHandleInternal();
	IIsolateHandle* ToHandle() override;

	///	Attempt to load compiled luau bytecode into the virtual machine.
	///	Returns true and writes to *method on success,
	///	false and writes to *error on failure.
	virtual bool TryLoad(const char* name, std::string data, IScriptMethod **method, char* error, int maxlength);

	IScriptFiber* NewFiber() final;

	ScriptFiber* GetFiber(int id);

public:
	lua_State *L;
protected:
	ScriptVM* _parent;
	std::vector<ScriptFiber*> _fibers;

protected:
	IScriptRef* thread_reference;
	int isolate_id;
};


#endif //DUOSCRIPT_SCRIPTVM_H
