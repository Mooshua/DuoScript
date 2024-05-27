// Copyright (c) 2024 Mooshua. All rights reserved.

#include "LuauCompilerController.h"

#include "Luau/Compiler.h"

#include <string>


LuauCompilerController g_LuauCompilerController;

IScriptResult *LuauCompilerController::TryCompile(IScriptCall *args)
{
	std::string source = std::string(1048576, '\0');
	if (!args->ArgString(1, source.data(), source.capacity()))
		return args->Error("Expected argument 1 to be a string!");

	try
	{
	} catch(Luau::CompileError error)
	{

	} catch(std::exception exception)
	{

	}
}
