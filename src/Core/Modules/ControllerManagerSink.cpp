// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "ControllerManagerSink.h"

ControllerManagerSink::ControllerManagerSink(IScriptControllerManager *upstream)
{
	this->upstream = upstream;
}

ControllerManagerSink::~ControllerManagerSink()
{
	for (IControllerInstance* item : this->controllers)
	{
		this->upstream->Destroy(item);
		delete item;
	}

	this->controllers.clear();
}

IControllerInstance *ControllerManagerSink::Register(IBaseScriptController *controller)
{
	IControllerInstance* instance = this->upstream->Register(controller);
	this->controllers.push_back(instance);

	return instance;
}

void ControllerManagerSink::Destroy(IControllerInstance *instance)
{
	//	TODO: make it so morons cant unregister controllers from other modules here
	//	BETTER IDEA: annihilate all morons
	this->upstream->Destroy(instance);

	//	God I hate C++
	auto remove = std::remove(this->controllers.begin(), this->controllers.end(), instance);
	this->controllers.erase(remove, this->controllers.end());

	//	Free metatable refs
	delete instance;
}
