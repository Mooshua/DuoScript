// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "DuoMetamod.h"

#include "mimalloc-new-delete.h"
#include "Logging/Log.h"
#include "Plugins/Plugin.h"
#include "Duo.h"
#include "IModule.h"
#include "Modules/ModuleManager.h"

#include "amtl/am-string.h"
#include "amtl/am-platform.h"

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

#ifdef KE_WINDOWS
	//	Windows is silly and stupid and by default disables virtual color handling.
	//	This has upsides and downsides. Upside is we don't need to eg. escape player names.
	//	The downside is no color text. And I need my color text >:[
	{
		HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD mode = 0;
		GetConsoleMode(out, &mode);
		SetConsoleMode(out, mode | ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	}
#endif

	this->is_late = late;

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

	g_Log.Component("Duo Core", Log::STAT_GOOD, "Started");

	return true;
}

bool DuoMetamod::Unload(char *error, size_t maxlen)
{
	g_Log.Component("Duo Core", Log::STAT_NOTE, "Stopping");
	delete g_Duo;

	if (!g_Loop.Destroy())
		return UTIL_Error(error, maxlen, "Unexpected loop shutdown error");

	g_Log.Message("Duo Core", Log::SEV_INFO, "Goodbye <3");

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
	//	All initial plugins loaded!
	g_Duo = new Duo(this->is_late);

	ISmmPlugin::AllPluginsLoaded();
}

void *DuoMetamod::OnMetamodQuery(const char *iface, int *ret)
{
	if (strcmp(iface, INTERFACE_MODULEINTRODUCER_001) == 0)
	{
		*ret = IFACE_OK;
		return static_cast<IModuleIntroducer*>(&g_ModuleManager);
	}

	*ret = IFACE_FAILED;
	return nullptr;
}