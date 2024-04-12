// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_IPLUGIN_H
#define DUOSCRIPT_IPLUGIN_H

#include <vector>
#include <string>
#include <FastDelegate.h>


class IPluginResources
{
public:
	///	Get the resource file with the specified name
	///	Writes an error string when returning false.
	///	You own the returned buffer--make sure to free it!
	virtual bool TryGetResource(const char* name, std::string* results) = 0;

	///	Get a list of resources contained in this plugin
	virtual void GetResources(std::vector<const char*>* names, const char* search = nullptr) = 0;
};

class IPlugin
{
public:
	///	Get a list of resources attached to this plugin
	virtual IPluginResources* GetResources() = 0;
};

class IPluginManager
{
	typedef fastdelegate::FastDelegate<void, IPlugin*, const char*> LoadCallback;
public:
	bool TryLoad(char* path, LoadCallback callback);
};

#endif //DUOSCRIPT_IPLUGIN_H
