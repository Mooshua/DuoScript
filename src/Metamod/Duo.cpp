// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "DuoMetamod.h"
#include "Duo.h"
#include "Plugins/PluginManager.h"
#include "Modules/ModuleManager.h"
#include "sourcehook/sourcehook.h"

Duo* g_Duo;

auto OnNetworkStartupServer = SourceHook::Hook<INetworkServerService, &INetworkServerService::StartupServer,
	void, const GameSessionConfiguration_t&, ISource2WorldSession*, const char*>::Make();

auto OnServerGameFrame = SourceHook::Hook<ISource2Server, &ISource2Server::GameFrame,
	void, bool, bool, bool>::Make();

Duo::Duo(bool late)
{
	//	Create the core duo object
	_core = new DuoCore();
	_log = _core->Log;

	//	This system actually loads in other duo bits for us
	_modules = new ModuleManager(_core);

	_log->Blank(" ┌──────────────────────────┐ ");
	_log->Blank(" │ DuoScript                │ ");
	_log->Blank(" └──────────────────────────┘ ");
	_log->Blank("Hello, World!");

	OnNetworkStartupServer->Add(g_NetworkServer, true, SH_MEMBER(this, &Duo::OnStartupServer));
	OnServerGameFrame->Add(g_Server, false, SH_MEMBER(this, &Duo::OnGameFrame));

	_log->Component("Duo Hooks", Log::STAT_GOOD, "Started");

	_started = false;

	if (late)
	{
		_log->Component("Duo Hooks", Log::STAT_NOTE, "Late load detected - Starting duo now!");
		this->Start();
	}
}

Duo::~Duo()
{
	OnNetworkStartupServer->Remove(g_NetworkServer, true, SH_MEMBER(this, &Duo::OnStartupServer));
	OnServerGameFrame->Remove(g_Server, false, SH_MEMBER(this, &Duo::OnGameFrame));

	this->Stop();

	g_Log.Component("Duo Hooks", Log::STAT_NOTE, "Stopped");
}

void Duo::Start()
{
	if (_started)
		return;

	_core->Loop->Initialize();
	_core->ScriptVM->Initialize();

	//	Create the core object
	_started = true;

	//	Load all other installed DuoMods
	_modules->LoadAllModules();

	//	Load and start all installed plugins
	_core->PluginManager->LoadAllPlugins();
}

void Duo::Stop()
{
	_modules.UnloadAllModules();
}


void Duo::OnStartupServer(const GameSessionConfiguration_t& config, ISource2WorldSession* _, const char* map)
{
	g_Log.Message("Duo Core", Log::SEV_INFO, "OnStartupServer! %s", map);

	this->Start();

	RETURN_META(MRES_IGNORED);
}

void Duo::OnGameFrame(bool simulating, bool first, bool last)
{
	//	Advance profiler
	FrameMark;
	tracy::SetThreadName("Main");

	duo::Frame _("DuoScript");

	//	Push events through the loop
	_core->Loop->RunOnce();

	RETURN_META(MRES_IGNORED);
}





