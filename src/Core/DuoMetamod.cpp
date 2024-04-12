// Copyright (c) 2024 Mooshua. All rights reserved.

#include "DuoMetamod.h"

#include "mimalloc-new-delete.h"
#include "Logging/Log.h"
#include "Plugins/Plugin.h"
#include "Duo.h"
#include "IModule.h"
#include "Modules/ModuleManager.h"

#include <amtl/am-string.h>

IVEngineServer2 *g_Engine;
ISource2Server *g_Server;
IFileSystem *g_Filesystem;
INetworkServerService *g_NetworkServer;

DuoMetamod g_DuoScript;

ISmmPluginManager* g_MetamodPluginManager;

PLUGIN_EXPOSE(DuoMetamod, g_DuoScript)

static bool UTIL_Error(char *err, size_t maxlen, const char* reason)
{
	ke::SafeSprintf(err, maxlen, "%s", reason);
	return false;
}

bool DuoMetamod::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	PLUGIN_SAVEVARS();

	//	Register ourselves as a listener so we can provide our interfaces
	//	to other plugins in the Metamod environment. See OnMetamodQuery
	ismm->AddListener(this, this);

	GET_V_IFACE_CURRENT(GetEngineFactory, g_Engine, IVEngineServer2, INTERFACEVERSION_VENGINESERVER);
	GET_V_IFACE_CURRENT(GetEngineFactory, g_NetworkServer, INetworkServerService, NETWORKSERVERSERVICE_INTERFACE_VERSION);

	GET_V_IFACE_CURRENT(GetServerFactory, g_Server, ISource2Server, INTERFACEVERSION_SERVERGAMEDLL);

	GET_V_IFACE_ANY(GetFileSystemFactory, g_Filesystem, IFileSystem, FILESYSTEM_INTERFACE_VERSION);

	{
		int ret;
		g_MetamodPluginManager =
			static_cast<ISmmPluginManager *>(ismm->MetaFactory(MMIFACE_PLMANAGER, &ret, &id));
	}

	g_Log.Message("Duo Core", Log::SEV_INFO, "Hello");

	//	Initialize core subsystems
	g_Loop.Initialize();
	g_ScriptVM.Initialize();

	g_Duo = new Duo();

	g_Log.Component("Duo Core", Log::STAT_GOOD, "Started");

	return true;
}

bool DuoMetamod::Unload(char *error, size_t maxlen)
{
	delete g_Duo;

	g_Log.Message("Metamod", Log::SEV_DEBUG, "Game path: %s", g_SMAPI->GetBaseDir());
	g_Log.Message("Metamod", Log::SEV_INFO, "Game path: %s", g_SMAPI->GetBaseDir());
	g_Log.Message("Metamod", Log::SEV_WARN, "Game path: %s", g_SMAPI->GetBaseDir());
	g_Log.Message("Metamod", Log::SEV_ERROR, "Game path: %s", g_SMAPI->GetBaseDir());

	g_Log.Component("Metamod", Log::STAT_GOOD, "Howdy");
	g_Log.Component("Metamod", Log::STAT_NOTE, "Howdy");
	g_Log.Component("Metamod", Log::STAT_FAIL, "Howdy");

	if (!g_Loop.Destroy())
		return UTIL_Error(error, maxlen, "Unexpected loop shutdown error");

	g_Log.Message("Duo Core", Log::SEV_INFO, "Goodbye o/");

	return true;
}

bool DuoMetamod::Pause(char *error, size_t maxlen)
{
	return ISmmPlugin::Pause(error, maxlen);
}

bool DuoMetamod::Unpause(char *error, size_t maxlen)
{
	return ISmmPlugin::Unpause(error, maxlen);
}

void DuoMetamod::AllPluginsLoaded()
{
	ISmmPlugin::AllPluginsLoaded();
}

void *DuoMetamod::OnMetamodQuery(const char *iface, int *ret)
{
	if (strcmp(iface, INTERFACE_MODULEINTRODUCER_001) == 0)
	{
		*ret = IFACE_OK;
		return static_cast<IModuleIntroducer*>(&g_ModuleOnboarding);
	}

	*ret = IFACE_FAILED;
	return nullptr;
}