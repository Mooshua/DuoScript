// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_ISCRIPTMETHOD_H
#define DUOSCRIPT_ISCRIPTMETHOD_H

class IScriptMethod;
class IScriptRef;
class IScriptObject;
class IScriptResult;
class IScript;
class IScriptIsolate;
class IScriptCall;

class IFiberHandle;
class IIsolateHandle;

#include "IScript.h"

class IScriptReturn
{
public:
	virtual int GetLength();
};

class IScriptInvoke
{
public:
	virtual IIsolateHandle *GetIsolate() = 0;
	virtual IFiberHandle *GetFiber() = 0;

public:
	virtual void PushBool(bool value) = 0;
	virtual void PushString(const char* value, int length = -1) = 0;
	virtual void PushObject(IScriptRef* value) = 0;
	virtual void PushNumber(double value) = 0;
	virtual void PushInt(int value) = 0;
	virtual void PushUnsigned(unsigned value) = 0;

	///	Push all arguments after the specified arguments.
	///	This can be used to pass varargs across C stacks.
	virtual void PushVarArgs(IScriptCall* from, int after = 1) = 0;
	virtual void PushArg(IScriptCall *from, int arg) = 0;

	///	Copy a buffer into Lua
	virtual void PushBuffer(void* data, size_t length) = 0;
};

///	Arguments passed to native code from lua
class IScriptCall : public virtual IScriptInvoke
{
public:

	///	Get the first argument (aka "self")
	virtual void* GetOpaqueSelf() = 0;

	///	Get the number of arguments available
	virtual int GetLength() = 0;

	///	Get the current namecall, if there is one.
	virtual bool TryGetNamecall(char* result, int maxlen) = 0;

	virtual bool ArgBool(int slot, bool *result) = 0;
	virtual bool ArgString(int slot, char* result, int maxlen, int* written = nullptr) = 0;
	virtual bool ArgObject(int slot, IScriptRef** result) = 0;
	virtual bool ArgNumber(int slot, double* result) = 0;
	virtual bool ArgInt(int slot, int* result) = 0;
	virtual bool ArgUnsigned(int slot, unsigned* result) = 0;
	virtual bool ArgBuffer(int slot, void** result, size_t* size) = 0;

	//	Get a method passed as an argument.
	//	Note that this can have upvalues and state--so don't send us
	//	across isolate boundaries!
	virtual bool ArgMethod(int slot, IScriptMethod** result) = 0;
	virtual bool ArgTable(int slot, IScriptObject** result) = 0;

public:
	///	Generate a response for this call that invokes an error
	///	in the calling method.
	virtual IScriptResult* Error(const char* error, ...) = 0;

	///	Suspend execution
	virtual IScriptResult* Await() = 0;

	virtual IScriptResult* Return() = 0;
public:
	template<typename Entity>
	inline Entity* GetSelf() { return static_cast<Entity*>(this->GetOpaqueSelf()); }
};

#endif //DUOSCRIPT_ISCRIPTMETHOD_H
