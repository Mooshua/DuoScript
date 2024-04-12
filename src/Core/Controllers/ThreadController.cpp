// Copyright (c) 2024 Mooshua. All rights reserved.

#include "ThreadController.h"

ThreadController g_ThreadController;

IScriptResult *ThreadController::Sleep(IScriptCall *args)
{
	unsigned duration;
	if (!args->ArgUnsigned(1, &duration))
		return args->Error("Expected argument 1 to be an unsigned integer");

	//	Begin delaying
	ThreadResumer *resumer = new ThreadResumer(args->GetFiber(), duration);
	return args->Await();
}

IScriptResult *ThreadController::Fiber(IScriptCall *args)
{
	if (!args->ArgMethod(1, nullptr))
		return args->Error("Expected argument 1 to be a method");

	IScriptIsolate* isolate = args->GetIsolate()->Get();
	IScriptFiber* fiber = isolate->NewFiber();

	IScriptInvoke* invoke;
	if (!fiber->TryContinue(&invoke))
		return args->Error("Cannot resume dead fiber!");
		//	Micro-optimization:
		//	To avoid creating a lua_ref just to chuck it
		//	across the barrier, we just assert the type
		//	(using ArgsMethod above) and use xmove.
		//invoke->PushObject(method);
		invoke->PushArg(args, 1);
		invoke->PushVarArgs(args, 2);
	fiber->Call(false);

	return args->Return();
}
