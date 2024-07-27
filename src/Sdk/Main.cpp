// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "Main.h"
#include "Logging/Log.h"
#include "Helpers/Path.h"

#include <iostream>

#include "Files/FileSystemController.h"
#include "Sdk/Controllers/LuauFrontend.h"
#include "Sdk/Controllers/LuauCheckResult.h"
#include "Memory/PointerController.h"
#include "Core.h"

#include "Logic.h"

duo::Plot g_PendingEvents("Pending Events", tracy::PlotFormatType::Number);


int main(int argc, char **argv)
{
	uv_setup_args(argc, argv);
	getchar();

	Main main;
	main.core.Loop->Initialize();
	main.core.ScriptVM->Initialize();

	DuoLogic logic(main.core.Log, main.core.Loop, main.core.Files );

	IScriptControllerManager* controllers = main.core.ScriptVM->controllers;
	{
		logic.OnScriptReady(controllers);

		controllers->Register(new LuauFrontendController(main.core.Log));
		controllers->Register(&g_LuauCheckResult);
	}

	main.Initialize();
	main.Run();
}

void Main::Initialize()
{

	char path[256];
	duo::BuildPath(path, sizeof(path), "%s/%s", duo::ExecutablePath().c_str(), "duokit.duo");

	this->_plugin = core.PluginManager->LoadPluginInternal("duokit.duo", path);

	if (!this->_plugin->success)
	{
		//	Error!
		g_Log.Component("DuoKit", ILogger::STAT_FAIL, "Error loading DuoKit: %s", _plugin->error.c_str());
		return;
	}

	this->_plugin->plugin->Start();
}

[[noreturn]] void Main::Run()
{
	while (true) {
		duo::Frame _("Loop");

		uv_run(core.Loop->AsLoop(), UV_RUN_ONCE);

		uv_metrics_t metrics;
		uv_metrics_info(core.Loop->AsLoop(), &metrics);

		g_PendingEvents.Set(metrics.events_waiting);
	}
}
