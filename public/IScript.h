// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under the Lesser GPL (see public/LICENSE.md), and defines an
// interface that may be linked to without distributing the linkee under a GPL license.

#ifndef DUOSCRIPT_ISCRIPT_H
#define DUOSCRIPT_ISCRIPT_H

class IScriptRef;
class IScriptObject;
class IScriptResult;
class IScript;
class IScriptIsolate;
class IScriptCall;

class IFiberHandle;
class IIsolateHandle;

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
	//	This should ONLY be called if you pass true to .Call().
	virtual ~IScriptFiber() = default;

	///	Create a handle pointing to this fiber
	///	This can be used to hold a weak reference
	///	that will dissolve when the fiber ends
	virtual IFiberHandle* ToHandle() = 0;

	///	True when the fiber is ready to begin another call
	virtual bool IsReady() = 0;

	///	Begin a call to the specified method
	///	Use the provided IScriptInvoke* to add arguments
	virtual bool TrySetup(IScriptMethod* method, IScriptInvoke** args = nullptr) = 0;

	///	Try to continue a previously yielded fiber
	///	This can be used by advanced users to start a coroutine within a fiber
	virtual bool TryContinue(IScriptInvoke** args = nullptr) = 0;

	///	Throw an error in this fiber from an external
	///	thread (do NOT call while executing!)
	virtual void Kill(const char* fmt, ...) = 0;

	///	Invoke this isolate once the appropriate TrySetup/TryContinue
	///	command has been issued & arguments have been passed
	///	@param use pass true to prevent the fiber from de-allocating itself.
	virtual IScriptReturn* Call(bool use = false) = 0;

	/// @brief Request another fiber be resumed when this one completes
	///	The other fiber should already be yielded. The fiber will be invoked
	///	with all args returned from this fiber.
	virtual bool TryDepend(IScriptFiber* other) = 0;
};

///	Resources for an isolate, such as require() resolving
class IIsolateResources
{
public:
	///	@brief Get the name of the isolate
	virtual const char* GetName() = 0;

	///	@brief Get the resource at the specified path
	virtual bool TryGetResource(const char* name, std::string* results) = 0;

	///	@brief Compile the source file at the specified path
	virtual bool TryGetCodeResource(const char *name, std::string *results) = 0;

};

class IScriptIsolate
{
public:

	///	Create a handle pointing to this isolate
	virtual IIsolateHandle* ToHandle() = 0;

	///	Get the resources associated with this isolate
	virtual IIsolateResources* GetResources() = 0;

	///	Load some code into this isolate
	///	This creates an IScriptMethod that can be executed in this isolate,
	///	or any for that matter, but it should really stay in here :)
	virtual bool TryLoad(const char* name, std::string data, IScriptMethod **method, char* error, int maxlength) = 0;

	///	Create a new fiber to execute code in
	///	Each fiber can run concurrently using Await() calls.
	virtual IScriptFiber* NewFiber() = 0;
};

class IScript
{
public:
	virtual void Initialize() = 0;

	virtual IScriptIsolate* CreateIsolate(IIsolateResources* resources) = 0;
};

class IIsolateHandle
{
public:
	virtual ~IIsolateHandle() = default;

	///	Determine if the object we are pointing to still exists
	virtual bool Exists() = 0;

	///	Get the object we are pointing to
	virtual IScriptIsolate* Get() = 0;
};

class IFiberHandle
{
public:
	virtual ~IFiberHandle() = default;

	///	Determine if the object we are pointing to still exists
	virtual bool Exists() = 0;

	///	Get the object we are pointing to
	virtual IScriptFiber* Get() = 0;
};

#endif //DUOSCRIPT_ISCRIPT_H
