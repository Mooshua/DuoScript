// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "Core.h"

DuoCore::DuoCore()
{
	this->Log = new class Log();
	this->Loop = new class Loop();
	this->Files = new class Files();

	this->ScriptVM = new class ScriptVM(Log);
	this->PluginManager = new class PluginManager(Log, Files, ScriptVM);
}