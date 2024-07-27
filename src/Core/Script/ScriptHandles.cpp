// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "ScriptHandles.h"
#include "ScriptVM.h"

IsolateHandle::IsolateHandle(ScriptVM *vm, int id)
{
	this->vm = vm;
	isolate_id = id;
}

bool IsolateHandle::Exists()
{
	DuoScope(IsolateHandle::Exists);

	return vm->GetIsolate(isolate_id) != nullptr;
}

IScriptIsolate * IsolateHandle::Get()
{
	return this->GetInternal();
}

ScriptIsolate *IsolateHandle::GetInternal()
{
	DuoScope(IsolateHandle::Get);

	return vm->GetIsolate(isolate_id);
}

FiberHandle::FiberHandle(ScriptVM* vm, IsolateHandle* parent, int id)
{
	this->vm = vm;
	fiber_id = id;
	isolate = parent;
}

bool FiberHandle::Exists()
{
	DuoScope(FiberHandle::Exists);

	if (!isolate->Exists())
		return false;

	return (isolate->GetInternal())->GetFiber(fiber_id) != nullptr;
}

void FiberHandle::Continue(IScriptReturn *with)
{
	//	erm....
	if (!this->Get())
		return;

	IScriptFiber* fiber = this->Get();
	IScriptInvoke *invoke;
	if (fiber->TryContinue(&invoke)) {
		//	First return: Was the fiber successful?
		invoke->PushBool(!with->IsError());
		invoke->PushVarArgs(with->ToPolyglot(), 1);

		fiber->Call(false);
	}

	delete this;
}

IScriptFiber *FiberHandle::Get()
{
	return this->GetInternal();
}

ScriptFiber *FiberHandle::GetInternal()
{
	DuoScope(FiberHandle::Get);

	return isolate->GetInternal()->GetFiber(fiber_id);
}

FiberHandle::~FiberHandle()
{
	delete isolate;
}
