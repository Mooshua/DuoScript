// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "FiberController.h"

IScriptResult *FiberController::New(IScriptCall *args)
{
	DuoScope(FiberController::New);

	if (!args->ArgMethod(1, nullptr))
		return args->Error("Expected argument 1 to be a method");

	IScriptIsolate* isolate = args->GetIsolate()->Get();
	IScriptFiber* fiber = isolate->NewFiber();

	//	Open a handle to the fiber & create entity
	IFiberHandle* handle = fiber->ToHandle();
	FiberEntity entity = FiberEntity(handle);

	IScriptInvoke* invoke;
	if (!fiber->TryContinue(&invoke))
		return args->Error("Cannot resume dead fiber!");
	{
		//	Micro-optimization:
		//	To avoid creating a lua_ref just to chuck it
		//	across the barrier, we just assert the type
		//	(using ArgsMethod above) and use xmove.
		//invoke->PushObject(method);
		invoke->PushArg(args, 1);
		invoke->PushVarArgs(args, 2);
	}
	fiber->Call(false);

	return this->ReturnNew(args, &entity);
}

IScriptResult *FiberController::Await(IScriptCall *args)
{
	DuoScope(FiberController::Await);

	//	Error if any arguments are passed
	//	This is to stop people from being silly and thinking this is
	//	similar to async/await patterns in other languages.
	if (args->GetLength() != 1)
		return args->Error(
			"Await() called with %i arguments! Don't pass arguments to Await()!!",
			args->GetLength() - 1);

	//	Log this
	_log->Tip("Fibers", "Await() invoked: make sure you don't deadlock!");

	return args->Await();
}

IScriptResult *FiberController::Resume(FiberEntity *entity, IScriptCall *args)
{
	DuoScope(FiberController::Resume);

	//	The fiber could be dead, even if we have a handle to it.
	if (!entity->fiber->Exists())
		return args->Error("Fiber handle %x is dead", entity);

	IScriptFiber* fiber = entity->fiber->Get();
	IScriptInvoke* invoke;
	if (!fiber->TryContinue(&invoke))
		return args->Error("Cannot resume dead fiber!");
	{
		invoke->PushVarArgs(args);
	}
	IScriptReturn* results = fiber->Call(true);

	//	Push varargs from the returned results.
	args->PushVarArgs(results, 0);

	return args->Return();
}

IScriptResult *FiberController::Sleep(IScriptCall *args)
{
	DuoScope(FiberController::Sleep);

	unsigned duration;
	ARG_UINT(args, 1, duration);


	//	Begin delaying
	new ThreadResumer(_loop, args->GetFiber(), duration);
	return args->Await();
}

IScriptResult *FiberController::Depend(FiberEntity *entity, IScriptCall *args)
{
	DuoScope(FiberController::Depend);

	if (!entity->fiber->Exists())
		return args->Error("Fiber does not exist!");

	IScriptFiber* fiber = entity->fiber->Get();

	if (!fiber->TryDepend(args->GetFiber()))
		return args->Error("Failed to depend on fiber!");

	return args->Await();
}
