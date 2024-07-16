// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#ifndef DUOSCRIPT_LUAUTYPE_H
#define DUOSCRIPT_LUAUTYPE_H

#include <IScriptController.h>

class LuauTypeEntity
{

};

class LuauTypeController : public IScriptController<LuauTypeEntity>
{
public:
	LuauTypeController()
	{
		CONTROLLER_NAME(LuauType);

	}

};


#endif //DUOSCRIPT_LUAUTYPE_H
