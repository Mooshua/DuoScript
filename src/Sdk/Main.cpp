// Copyright (c) 2024 Mooshua. All rights reserved.

#include "Main.h"
#include "Logging/Log.h"

#include "Threading/FiberController.h"
#include "Files/FileSystemController.h"
#include "Generic/LogController.h"
#include "Sdk/Controllers/LuauFrontend.h"
#include "Sdk/Controllers/LuauCheckResult.h"
#include "Files/ZipController.h"

Main g_Main;
ILogger* g_DuoLog;
ILoop* g_DuoLoop;

int main(int argc, char *argv[])
{
	//	This is to provide binary build compatibility
	//	with duo_logic
	g_DuoLog = &g_Log;
	g_DuoLoop = &g_Loop;

	g_Loop.Initialize();
	g_ScriptVM.Initialize();

	IScriptControllerManager* controllers = g_ScriptVM.controllers;
	{
		controllers->Register(&g_FiberController);
		controllers->Register(&g_FileController);
		controllers->Register(&g_LogController);
		controllers->Register(&g_ZipController);

		controllers->Register(&g_LuauFrontend);
		controllers->Register(&g_LuauCheckResult);

	}

	g_Main.Initialize();
	g_Main.Run();
}

void Main::Initialize()
{
	this->_plugin = g_PluginManager.LoadPlugin("duokit.duo");

	if (!this->_plugin->success)
	{
		//	Error!
		g_Log.Component("DuoKit", ILogger::STAT_FAIL, "Error loading DuoKit: %s", _plugin->error.c_str());
		return;
	}

	this->_plugin->plugin->Start();
}

void Main::Run()
{
	uv_run(g_Loop.AsLoop(), UV_RUN_DEFAULT);
}
