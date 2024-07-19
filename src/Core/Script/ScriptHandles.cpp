// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "ScriptHandles.h"
#include "ScriptVM.h"

IsolateHandle::IsolateHandle(int id)
{
	isolate_id = id;
}

bool IsolateHandle::Exists()
{
	DuoScope(IsolateHandle::Exists);

	return g_ScriptVM.GetIsolate(isolate_id) != nullptr;
}

IScriptIsolate * IsolateHandle::Get()
{
	return this->GetInternal();
}

ScriptIsolate *IsolateHandle::GetInternal()
{
	DuoScope(IsolateHandle::Get);

	return g_ScriptVM.GetIsolate(isolate_id);
}

FiberHandle::FiberHandle(IsolateHandle* parent, int id)
{
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
