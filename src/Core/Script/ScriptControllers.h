// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_SCRIPTCONTROLLERS_H
#define DUOSCRIPT_SCRIPTCONTROLLERS_H


#include "IScriptController.h"
#include "lua.h"
#include "lualib.h"

class ControllerInstance : public virtual IControllerInstance
{
public:
	ControllerInstance(IScriptRef* metatable, IScriptRef* staticMetatable);
	virtual ~ControllerInstance();

public:
	void Kill() override { _killed = true; }

protected:
	virtual IScriptRef* Make(IScriptIsolate* vm, void** result, int size);

	IScriptRef* GetMetatable() override { return metatable; }
	IScriptRef* GetStaticMetatable() override { return staticMetatable; }
	bool IsAlive() override { return !_killed; };

protected:
	///	Set to true when the underlying controller object
	///	has been killed. Used to prevent accessing deallocated memory.
	bool _killed;

	IScriptRef* metatable;
	IScriptRef* staticMetatable;
};

class ScriptControllerManager : public virtual IScriptControllerManager
{
	friend class ControllerInstance;
public:
	ScriptControllerManager(lua_State *L)
		: L(L)
	{

	}
public:
	IControllerInstance* Register(IBaseScriptController* controller) override;

	void Destroy(IControllerInstance* instance) override;

protected:
	///	Destructor provided to the Luau runtime
	static void OnDestroy(void* userdata);

protected:
	///	lua_ref to this->InvokeUserdataNamecall
	lua_State *L;
	int ref_namecall_func;
};


#endif //DUOSCRIPT_SCRIPTCONTROLLERS_H
