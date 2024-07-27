// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#ifndef DUOSCRIPT_PLUGINFILECONTROLLER_H
#define DUOSCRIPT_PLUGINFILECONTROLLER_H

#include <IScriptController.h>

class PluginFileEntity : public IFiberContinuation
{
public:	//	IFiberContinuation
	void Continue(IScriptReturn* with);

public:
	PluginFileEntity(IIsolateHandle* owner, std::string path)
	: _owner(owner)
	, _path(path)
	{

	}

	///	@brief the owner of this file
	IIsolateHandle* _owner;

	///	@brief has this file been executed?
	bool _executeStarted = false;
	bool _executeFinished = false;

	IFiberHandle* _executeHandle;

	///	@brief if this file has been executed, the results will be here
	std::vector<IScriptRef*> _executeResults;

	///	@brief The path to the file
	std::string _path;
};

class PluginFileController : public IScriptController<PluginFileEntity>
{
	friend class PluginSingletonController;
public:
	PluginFileController()
	{
		CONTROLLER_NAME(PluginFile);
		CONTROLLER_METHOD(Import, &PluginFileController::Import);
	}

	IScriptResult* Import(PluginFileEntity* entity, IScriptCall* call);
};


#endif //DUOSCRIPT_PLUGINFILECONTROLLER_H
