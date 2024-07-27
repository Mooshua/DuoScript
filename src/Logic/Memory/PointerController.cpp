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
	ARG_INT(call, 1, offset);

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
	ARG_UINT(call, 1, length);

	//	Luau will now make a !!FULL COPY!! of the data and pass it to the
	//	scripting engine. The luau buffer library can now be used to read
	//	the data however the consumer pleases :>
	call->PushBuffer(*entity, length);
	return call->Return();
}

IScriptResult *PointerController::Write(void* *entity, IScriptCall *call)
{
	std::string buffer;
	ARG_BUFFER(call, 1, buffer);

	memcpy(*entity, buffer.data(), buffer.size());
	return call->Return();
}

IScriptResult *PointerController::OfModule(IScriptCall *call)
{
	std::string module_name;
	ARG_STRING(call, 1, module_name);

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

