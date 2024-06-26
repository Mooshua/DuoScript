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
	OnNetworkStartupServer->Add(g_NetworkServer, true, SH_MEMBER(this, &Duo::OnStartupServer));
	OnServerGameFrame->Add(g_Server, false, SH_MEMBER(this, &Duo::OnGameFrame));

	g_Log.Component("Duo Hooks", Log::STAT_GOOD, "Started");

	_started = false;

	if (late)
	{
		g_Log.Component("Duo Hooks", Log::STAT_NOTE, "Late load detected");
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

	_started = true;

	g_ModuleManager.LoadAllModules();
	g_PluginManager.LoadAllPlugins();
}

void Duo::Stop()
{
	g_ModuleManager.UnloadAllModules();
}


void Duo::OnStartupServer(const GameSessionConfiguration_t& config, ISource2WorldSession* _, const char* map)
{
	g_Log.Message("Duo Core", Log::SEV_INFO, "OnStartupServer! %s", map);

	this->Start();

	RETURN_META(MRES_IGNORED);
}

void Duo::OnGameFrame(bool simulating, bool first, bool last)
{
	//	Push events through the loop
	g_Loop.RunOnce();

	RETURN_META(MRES_IGNORED);
}





