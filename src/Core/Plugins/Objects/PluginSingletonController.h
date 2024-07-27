// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#ifndef DUOSCRIPT_PLUGINSINGLETONCONTROLLER_H
#define DUOSCRIPT_PLUGINSINGLETONCONTROLLER_H

#include <IScriptController.h>
#include "PluginFileController.h"

class PluginFolder
{
public:
	explicit PluginFolder(const std::string &path);

public:
	std::string path;
};

class PluginSingletonController : IScriptController<PluginFolder>
{
protected:
	PluginFileController* _fileController;
public:
	PluginSingletonController(PluginFileController* fileController)
	:	_fileController(fileController)
	{
		CONTROLLER_NAME(Plugin);
	}

	virtual IScriptResult* Index(IScriptCall *index) override;

	virtual IScriptResult* IndexStatic(IScriptCall *index) override;
protected:
	IScriptResult* Handle(const std::string& path, IScriptCall* index);

};


#endif //DUOSCRIPT_PLUGINSINGLETONCONTROLLER_H
