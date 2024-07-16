// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "LuauCompilerController.h"

#include "Luau/Compiler.h"

#include <string>


LuauCompilerController g_LuauCompilerController;

IScriptResult *LuauCompilerController::TryCompile(IScriptCall *args)
{
	std::string source = std::string();
	if (!args->ArgString(1, &source))
		return args->Error("Expected argument 1 to be a string!");

	try
	{
	} catch(Luau::CompileError error)
	{

	} catch(std::exception exception)
	{

	}
}