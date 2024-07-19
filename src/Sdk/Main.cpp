// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "Main.h"
#include "Logging/Log.h"
#include "Helpers/Path.h"

#include <iostream>

#include "Threading/FiberController.h"
#include "Files/FileSystemController.h"
#include "Generic/LogController.h"
#include "Sdk/Controllers/LuauFrontend.h"
#include "Sdk/Controllers/LuauCheckResult.h"
#include "Files/ZipController.h"
#include "Memory/PointerController.h"

duo::Plot g_PendingEvents("Pending Events", tracy::PlotFormatType::Number);
Main g_Main;
ILogger* g_DuoLog;
ILoop* g_DuoLoop;

int main(int argc, char **argv)
{
	uv_setup_args(argc, argv);
	getchar();

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
		controllers->Register(&g_PointerController);

		controllers->Register(&g_LuauFrontend);
		controllers->Register(&g_LuauCheckResult);

	}

	g_Main.Initialize();
	g_Main.Run();
}

void Main::Initialize()
{

	char path[256];
	duo::BuildPath(path, sizeof(path), "%s/%s", duo::ExecutablePath().c_str(), "duokit.duo");

	this->_plugin = g_PluginManager.LoadPluginInternal("duokit.duo", path);

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
	while (true) {
		duo::Frame _("Loop");

		uv_run(g_Loop.AsLoop(), UV_RUN_ONCE);

		uv_metrics_t metrics;
		uv_metrics_info(g_Loop.AsLoop(), &metrics);

		g_PendingEvents.Set(metrics.events_waiting);
	}
}
