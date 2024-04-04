// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_ISCRIPT_H
#define DUOSCRIPT_ISCRIPT_H

class IScriptRef;
class IScriptObject;
class IScriptResult;
class IScript;
class IScriptCall;


#include <FastDelegate.h>

#include <amtl/am-platform.h>
#include <amtl/am-hashmap.h>

#include <string>

#include "ILoop.h"
#include "IScriptMethod.h"

class IScriptResult
{
public:
};

///	A generic reference to an opaque type
class IScriptRef
{
public:
	virtual ~IScriptRef() = default;

public:
	virtual int AsReferenceId() = 0;
	virtual IScriptRef* Clone() = 0;
};

class IScriptMethod : public virtual IScriptRef
{
public:

};

///	A script object allows you to reference items that exist
///	in the script world. You can safely hold on to a script object
///	for extended durations of time, but it WILL prevent all related
///	items from being garbage collected!
class IScriptObject : public virtual IScriptRef
{
public:
	virtual bool TryGetNumber(const char* key, double *result) = 0;
	virtual bool TryGetNumber(int index      , double *result) = 0;

	virtual bool TryGetString(const char* key, std::string *result) = 0;
	virtual bool TryGetString(int index      , std::string *result) = 0;

	virtual bool TryGetObject(const char* key, IScriptObject **result) = 0;
	virtual bool TryGetObject(int index      , IScriptObject **result) = 0;
};


class IScriptFiber
{
public:
	///	True when the fiber is ready to begin another call
	virtual bool IsReady() = 0;

	///	Begin a call to the specified method
	virtual bool TrySetup(IScriptMethod* method) = 0;

	///	Invoke this isolate
	virtual IScriptCall* Call() = 0;
};

class IScriptIsolate
{
public:
	virtual bool TryLoad(const char* name, const char* data, size_t length, IScriptMethod **method, char ** error, int maxlength) = 0;

	virtual IScriptFiber* NewFiber() = 0;
};

class IScriptReturn
{
public:
	virtual int GetLength();
};

///	Arguments passed to native code from lua
class IScriptCall
{
public:
	virtual IScriptIsolate *GetIsolate() = 0;

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

	virtual void PushBool(bool value) = 0;
	virtual void PushString(const char* value, int length = -1) = 0;
	virtual void PushObject(IScriptRef* value) = 0;
	virtual void PushNumber(double value) = 0;
	virtual void PushInt(int value) = 0;
	virtual void PushUnsigned(unsigned value) = 0;

public:
	///	Generate a response for this call that invokes an error
	///	in the calling method.
	virtual IScriptResult* Error(const char* error) = 0;

	///	Suspend execution
	virtual IScriptResult* Yield() = 0;

	virtual IScriptResult* Return() = 0;
public:
	template<typename Entity>
	inline Entity* GetSelf() { return static_cast<Entity*>(this->GetOpaqueSelf()); }
};

class IScript
{
public:
	virtual void Initialize() = 0;

	virtual IScriptIsolate* CreateIsolate() = 0;
};

#endif //DUOSCRIPT_ISCRIPT_H
