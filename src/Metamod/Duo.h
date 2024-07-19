// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#ifndef DUOSCRIPT_DUO_H
#define DUOSCRIPT_DUO_H

#include "iserver.h"
#include "entitysystem.h"

#include "IProfiler.h"

#include "Loop/Loop.h"
#include "Script/ScriptVM.h"
#include "Logging/Log.h"

//	Stupid dumb stuff
class GameSessionConfiguration_t { };

class Duo
{
public:
	Duo(bool late);
	~Duo();
protected:	//	Handlers
	void Start();
	void Stop();

protected:
	bool _started;

protected:	//	Hooks
	void OnStartupServer(const GameSessionConfiguration_t& config, ISource2WorldSession* _, const char* map);
	void OnGameFrame(bool simulating, bool first, bool last);
};

extern Duo* g_Duo;


#endif //DUOSCRIPT_DUO_H
