// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "PluginSingletonController.h"
#include "amtl/am-string.h"

IScriptResult *PluginSingletonController::Index(IScriptCall *index)
{
	std::string key;
	if (!index->ArgString(1, &key))
		return index->Error("Invalid __index invocation");

	PluginFolder* self = index->GetSelf<Userdata>()->AsEntity();
	if (self == nullptr)
		return index->Error("Invalid __index invocation (null entity)");

	std::string newPath = ke::StringPrintf("%s/%s", self->path.c_str(), key.c_str());

	return this->Handle(newPath, index);
}

IScriptResult *PluginSingletonController::IndexStatic(IScriptCall *index)
{
	std::string key;
	if (!index->ArgString(1, &key))
		return index->Error("Invalid __index invocation");

	return this->Handle(key, index);
}

IScriptResult *PluginSingletonController::Handle(const std::string& path, IScriptCall *index)
{
	//	Check if path is valid?
	IScriptIsolate* isolate = index->GetIsolate()->Get();
	std::string result;
	auto type = isolate->GetResources()->TryGetResource(path.c_str(), &result);

	if (type == IIsolateResources::File) {
		//	This is a file! Return a new PluginFile
		PluginFileEntity entity(index->GetIsolate(), path);
		return _fileController->ReturnNew(index, &entity);
	} else if (type == IIsolateResources::Directory) {
		PluginFolder entity(path);
		return this->ReturnNew(index, &entity);
	}

	return index->Error("Could not find file '%s'", path.c_str());
}

PluginFolder::PluginFolder(const std::string &path) : path(path)
{}
