// Copyright (c) 2024 Mooshua. All rights reserved.

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
	//	TODO: make it so morons cant unregister controllers from other plugins here
	//	BETTER IDEA: anhillate all morons
	this->upstream->Destroy(instance);

	//	God I hate C++
	auto remove = std::remove(this->controllers.begin(), this->controllers.end(), instance);
	this->controllers.erase(remove, this->controllers.end());

	//	Free metatable refs
	delete instance;
}
