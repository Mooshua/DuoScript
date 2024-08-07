// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#ifndef DUOSCRIPT_SCRIPTHANDLES_H
#define DUOSCRIPT_SCRIPTHANDLES_H

class ScriptIsolate;
class ScriptFiber;

#include <IScript.h>
#include "ScriptVM.h"

class IsolateHandle : public virtual IIsolateHandle
{
public:
	IsolateHandle(ScriptVM *vm, int id);
	virtual ~IsolateHandle() = default;

	///	Determine if the object we are pointing to still exists
	virtual bool Exists();

	///	Get the object we are pointing to
	ScriptIsolate* GetInternal();
	virtual IScriptIsolate* Get();

protected:
	ScriptVM* vm;
	int isolate_id;
};

class FiberHandle : public virtual IFiberHandle
{
public:	//	IFiberContinuation
	virtual void Continue(IScriptReturn* with);

public:
	FiberHandle(ScriptVM* vm, IsolateHandle* parent, int id);
	virtual ~FiberHandle();

	///	Determine if the object we are pointing to still exists
	virtual bool Exists();

	///	Get the object we are pointing to
	ScriptFiber* GetInternal();
	virtual IScriptFiber* Get();


protected:
	ScriptVM* vm;
	IsolateHandle* isolate;
	int fiber_id;
};



#endif //DUOSCRIPT_SCRIPTHANDLES_H
