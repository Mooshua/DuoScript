// Copyright (c) 2024 Mooshua. All rights reserved.

#include "DuoMetamod.h"
#include "Duo.h"
#include "Plugins/PluginManager.h"
#include <sourcehook.h>

Duo* g_Duo;

SH_DECL_HOOK3_void(INetworkServerService, StartupServer, SH_NOATTRIB, 0, const GameSessionConfiguration_t&, ISource2WorldSession*, const char*);
SH_DECL_HOOK3_void(ISource2Server, GameFrame, SH_NOATTRIB, 0, bool, bool, bool);

Duo::Duo()
{
	SH_ADD_HOOK(INetworkServerService, StartupServer, g_NetworkServer, SH_MEMBER(this, &Duo::OnStartupServer), false);
	SH_ADD_HOOK(ISource2Server, GameFrame, g_Server, SH_MEMBER(this, &Duo::OnGameFrame), false);

	g_Log.Component("Duo Hooks", Log::STAT_GOOD, "Started");
}

Duo::~Duo()
{
	SH_REMOVE_HOOK(INetworkServerService, StartupServer, g_NetworkServer, SH_MEMBER(this, &Duo::OnStartupServer), false);
	SH_REMOVE_HOOK(ISource2Server, GameFrame, g_Server, SH_MEMBER(this, &Duo::OnGameFrame), false);

	g_Log.Component("Duo Hooks", Log::STAT_NOTE, "Stopped");
}

void Duo::OnStartupServer(const GameSessionConfiguration_t& config, ISource2WorldSession* _, const char* map)
{
	g_Log.Message("Duo Core", Log::SEV_INFO, "OnStartupServer! %s", map);

	g_PluginManager.LoadAllPlugins();

	RETURN_META(MRES_IGNORED);
}

void Duo::OnGameFrame(bool simulating, bool first, bool last)
{
	//	Push events through the loop
	g_Loop.RunOnce();

	RETURN_META(MRES_IGNORED);
}


