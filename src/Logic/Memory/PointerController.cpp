// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "PointerController.h"
#include "amtl/am-string.h"

#ifdef KE_WINDOWS
#include <windows.h>
#endif

PointerController g_PointerController;

IScriptResult *PointerController::Offset(void **entity, IScriptCall *call)
{
	int offset;
	if (!call->ArgInt(1, &offset))
		return call->Error("Expected argument 1 to be an offset!");

	void* result = static_cast<char*>(*entity) + offset;
	return this->ReturnNew(call, &result);
}

IScriptResult *PointerController::Dereference(void* *entity, IScriptCall *call)
{
	//	TODO: Basic memory safety required here!
	void* result(* static_cast<void**>(*entity));

	return this->ReturnNew(call, &result);
}

IScriptResult *PointerController::RipRelDereference(void* *entity, IScriptCall *call)
{
	//	TODO: Basic memory safety required here!
	//	RIP-relative dereference
	//	Pointer = Pointer + (int32)*Pointer + 4
	int32_t relative = *static_cast<int*>(*entity);

	void* result( static_cast<char*>(*entity) + relative + 4 );

	return this->ReturnNew(call, &result);
}

IScriptResult *PointerController::Read(void* *entity, IScriptCall *call)
{
	//	TODO: Basic memory safety required here!
	//	Create a buffer from the length specified
	unsigned length;
	if (!call->ArgUnsigned(1, &length))
		return call->Error("Expected argument 1 to be a length!");

	//	Luau will now make a !!FULL COPY!! of the data and pass it to the
	//	scripting engine. The luau buffer library can now be used to read
	//	the data however the consumer pleases :>
	call->PushBuffer(*entity, length);
	return call->Return();
}

IScriptResult *PointerController::Write(void* *entity, IScriptCall *call)
{
	void* result;
	size_t size;
	if (!call->ArgBuffer(1, &result, &size))
		return call->Error("Expected argument 1 to be a buffer!");

	memcpy(*entity, result, size);
	return call->Return();
}

IScriptResult *PointerController::OfModule(IScriptCall *call)
{
	std::string module_name;
	if (!call->ArgString(1, &module_name))
		return call->Error("Expected argument 1 to be a string!");

#ifdef KE_WINDOWS
	void* module = GetModuleHandleA(module_name.c_str());
	if (!module)
		return call->Error("[Win32] Failed to get module handle for %s", module_name.c_str());
#else
#error "Implement PointerController::OfModule for platform"
#endif

	return this->ReturnNew(call, &module);
}

IScriptResult *PointerController::ToString(void **entity, IScriptCall *call)
{
	std::string result = ke::StringPrintf("Pointer <0x%zp>", *entity);

	call->PushString(result.data(), result.length());
	return call->Return();
}

