// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#ifndef DUOSCRIPT_POINTERCONTROLLER_H
#define DUOSCRIPT_POINTERCONTROLLER_H

#include <IScriptController.h>

///	Pointers - An immutable utility for reading and writing process memory
///	Each pointer is fully immutable--only writable on construct--and can be
///	manipulated using a fluent-like API. (ptr:Deref():RelDeref())
///	Yes this increases GC pressure, but only slightly! :)
class PointerController : public IScriptController<void*>
{
public:
	PointerController()
	{
		CONTROLLER_NAME(Pointer);

		CONTROLLER_STATIC_METHOD(OfModule, &PointerController::OfModule);

		CONTROLLER_METHOD(Offset, &PointerController::Offset);

		CONTROLLER_METHOD(Deref, &PointerController::Dereference);
		CONTROLLER_METHOD(RelDeref, &PointerController::RipRelDereference);

		CONTROLLER_METHOD(Read, &PointerController::Read);
		CONTROLLER_METHOD(Write, &PointerController::Write);

		CONTROLLER_METHOD(ToString, &PointerController::ToString);
	}

public:

	///	Get a pointer from a module name
	IScriptResult* OfModule(IScriptCall* call);

	IScriptResult* Offset(void** entity, IScriptCall* call);

	IScriptResult* Dereference(void** entity, IScriptCall* call);
	IScriptResult* RipRelDereference(void** entity, IScriptCall* call);

	IScriptResult* Read(void** entity, IScriptCall* call);
	IScriptResult* Write(void** entity, IScriptCall* call);

	IScriptResult* ToString(void** entity, IScriptCall* call);

};

extern PointerController g_PointerController;


#endif //DUOSCRIPT_POINTERCONTROLLER_H
