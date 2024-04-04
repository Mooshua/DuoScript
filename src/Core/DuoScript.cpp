// Copyright (c) 2024 Mooshua. All rights reserved.

#include "DuoScript.h"

ISource2Engine *g_Engine;
ISource2Server *g_Server;

DuoScript g_DuoScript;

PLUGIN_EXPOSE(DuoScript, g_DuoScript)

static bool UTIL_Error(char *err, size_t maxlen, const char* reason)
{
	strcpy_s(err, maxlen, reason);
	return false;
}

bool DuoScript::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	PLUGIN_SAVEVARS();

	//	Register ourselves as a listener so we can provide our interfaces
	//	to other plugins in the Metamod environment. See OnMetamodQuery
	ismm->AddListener(this, this);

	GET_V_IFACE_CURRENT(GetEngineFactory, g_Engine, ISource2Engine, INTERFACEVERSION_VENGINESERVER);
	GET_V_IFACE_CURRENT(GetServerFactory, g_Server, ISource2Server, INTERFACEVERSION_SERVERGAMEDLL);

	//	Initialize core subsystems
	g_Loop.Initialize();

	return true;
}

bool DuoScript::Unload(char *error, size_t maxlen)
{
	if (!g_Loop.Destroy())
		return UTIL_Error(error, maxlen, "Unexpected loop shutdown error");

	return true;
}

bool DuoScript::Pause(char *error, size_t maxlen)
{
	return ISmmPlugin::Pause(error, maxlen);
}

bool DuoScript::Unpause(char *error, size_t maxlen)
{
	return ISmmPlugin::Unpause(error, maxlen);
}

void DuoScript::AllPluginsLoaded()
{
	ISmmPlugin::AllPluginsLoaded();
}

void *DuoScript::OnMetamodQuery(const char *iface, int *ret)
{
	return IMetamodListener::OnMetamodQuery(iface, ret);
}
