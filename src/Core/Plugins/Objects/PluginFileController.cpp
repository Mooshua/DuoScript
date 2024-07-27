// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "PluginFileController.h"

IScriptResult *PluginFileController::Import(PluginFileEntity *entity, IScriptCall *call)
{
	DuoScope(PluginFile::Import);
	IScriptIsolate* owner = entity->_owner->Get();

	if (!owner)
		return call->Error("PluginFile belongs to nonexistant isolate");

	if (entity->_executeStarted)
	{
		if (entity->_executeFinished)
		{
			//	We have finished execution, so we use all cached results
			for (auto ref : entity->_executeResults)
				call->PushObject(ref);

			return call->Return();
		}

		//	We are still in progress, no results yet :(
		//	Depend on the results of the fiber.
		if (!entity->_executeHandle || !entity->_executeHandle->Get())
			return call->Error("File '%s' execution \"started\" but there is no fiber handle available!", entity->_path.c_str());

		IScriptFiber* fiber = entity->_executeHandle->Get();
		if (!fiber->TryDepend(call->GetFiber()))
			return call->Error("Failed to depend on fiber");

		return call->Await();
	}

	//	No execution started yet!
	//	Start execution now :>
	IIsolateResources* resources = owner->GetResources();
	std::string contents;

	if (!resources->TryGetCodeResource(entity->_path.c_str(), &contents))
		return call->Error("Error reading file '%s': %s", entity->_path.c_str(), contents.c_str());

	//	Load the script into the isolate
	char error[1024];
	IScriptMethod* method;
	if (!owner->TryLoad("index.luau", contents, &method, error, sizeof(error)))
		return call->Error("Error loading file '%s': %s", entity->_path.c_str(), error);

	IScriptFiber* fileFiber = owner->NewFiber();

	//	Set local variables first, race conditiony!
	entity->_executeHandle = fileFiber->ToHandle();
	entity->_executeStarted = true;

	fileFiber->TrySetup(method);
	auto results = fileFiber->Call(true);

	//	POSSIBLE BUG SPOT:
	//	If we depend here and then yield, if the plugin runs to completion
	//	Then we have just yielded without having control of the plugin
	//	(as we passed control to the dependent)
	//	bleghhhhhh I should really make a helper func for this

	if (results->IsYielding())
	{
		//	Fiber yielded, we yield too
		IScriptFiber* fiber = entity->_executeHandle->Get();
		if (!fiber->TryDepend(call->GetFiber()))
			return call->Error("Failed to depend on fiber");

		return call->Await();
	}

	//	Ran to completion, return results
	call->PushBool(!results->IsError());
	if (results->IsError())
		call->PushString(results->GetError());
	else
		call->PushVarArgs(results);

	delete fileFiber;
	return call->Return();
}

void PluginFileEntity::Continue(IScriptReturn *with)
{
	DuoScope(PluginFile::Import - Continuation);

	//	Save the results from the run
	for (int i = 1; i <= with->GetLength(); ++i)
	{
		IScriptRef* ref;
		with->ArgObject(i, &ref);
		_executeResults.push_back(ref);
	}

	_executeFinished = true;
	_executeHandle = nullptr;
}
