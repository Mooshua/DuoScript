// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_DUOMETAMOD_H
#define DUOSCRIPT_DUOMETAMOD_H

#include "uv.h"

#include "ISmmPlugin.h"
#include "ISmmAPI.h"

#include "interfaces/interfaces.h"
#include "eiface.h"
#include "filesystem.h"

#include "Core/Loop/Loop.h"
#include "Script/ScriptVM.h"

#if (_MSC_VER <1300)
#error "Hey, buddy, it's 2024 pal"
#endif

extern IVEngineServer2 *g_Engine;
extern ISource2Server *g_Server;
extern IFileSystem *g_Filesystem;
extern INetworkServerService *g_NetworkServer;

extern ISmmPluginManager* g_MetamodPluginManager;

PLUGIN_GLOBALVARS();

class DuoMetamod : public ISmmPlugin, public IMetamodListener
{
public:
	bool Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late) override;
	bool Unload(char *error, size_t maxlen) override;
	bool Pause(char *error, size_t maxlen) override;
	bool Unpause(char *error, size_t maxlen) override;
	void AllPluginsLoaded() override;

public:
	void *OnMetamodQuery(const char* iface, int *ret);

protected:
	bool is_late;

public:
	const char *GetAuthor() override { return "Mooshua"; };
	const char *GetName() override { return "[DuoMod] DuoScript"; };
	const char *GetDescription() override { return "Modding platform for Counter-Strike 2"; };
	const char *GetURL() override { return "https://mooshua.net/"; };
	const char *GetLicense() override { return "GPLv3"; };
	const char *GetVersion() override { return "0.0.0"; };
	const char *GetDate() override { return "0/0/2024"; };
	const char *GetLogTag() override { return "DUO"; };
};

extern DuoMetamod g_DuoScript;

#endif //DUOSCRIPT_DUOMETAMOD_H
