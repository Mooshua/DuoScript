// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "Plugin.h"
#include "Logging/Log.h"
#include "Luau/Compiler.h"

#include <amtl/am-string.h>

Plugin::~Plugin()
{
	zip_stream_close(this->_file);
}

Plugin::Plugin(ScriptVM *vm, const char *path, PluginManager* parent, int id)
{
	this->_path = path;
	this->_name = this->_path.substr(this->_path.find_last_of("/\\") + 1);

	this->_parent = parent;
	this->_id = id;

	this->_file = zip_open(path, 0, 'r');
	this->_isolate = vm->CreateIsolateInternal(this);

	this->_loaded = false;
	this->_entryMethod = nullptr;
}

bool Plugin::TryGetCodeResource(const char *name, std::string *results)
{
	if (this->TryGetResource(name, results) != ResourceType::File)
		return false;

	//	Okay, now we have our program in results*.
	//	Make a new string with that
	std::string code = *results;

	Luau::CompileOptions opts;
	opts.coverageLevel = 2;
	opts.debugLevel = 2;
	opts.optimizationLevel = 2;

	//	Now compile with results as output
	*results = Luau::compile(code, opts);

	//	Note: Once compiler is invoked, we don't fail.
	//	Compiler errors will just be passed on to the runtime when we load the code.
	//	Neato thing from Luau.
	return true;
}


bool Plugin::TryLoad(char *error, int maxlen)
{
	std::string contents;

	if (!this->TryGetCodeResource("index.luau", &contents))
	{
		ke::SafeStrcpy(error, maxlen, contents.c_str());
		return false;
	}

	//	Load the script into the isolate
	IScriptMethod* method;
	if (!this->_isolate->TryLoad("index.luau", contents, &method, error, maxlen))
		return false;

	//	"Method" is used to boot the plugin, so to speak.
	this->_loaded = true;
	this->_entryMethod = method;
	return true;
}

IIsolateResources::ResourceType Plugin::TryGetResource(const char *name, std::string *results)
{
	DuoScope(Plugin::TryGetResource);

	int open = zip_entry_opencasesensitive(this->_file, name);
	if (open != 0)
	{
		//	Failure getting file. Write error to results
		*results = std::string(zip_strerror(open));
		return ResourceType::Nonexistant;
	}

	if (zip_entry_isdir(this->_file))
	{
		zip_entry_close(this->_file);
		return ResourceType::Directory;
	}

	size_t size = zip_entry_size(this->_file);
	*results = std::string(size + 2, '\0');

	//	This only fails when this->_file is uninitialized/closed
	//	or when we're out of memory. So we don't care about the return :)
	zip_entry_noallocread(this->_file, results->data(), size);


	zip_entry_close(this->_file);
	return ResourceType::File;
}

void Plugin::GetResources(std::vector<const char *> *names, const char *search)
{

}

IPluginResources *Plugin::GetResources()
{
	//	tee hee
	return this;
}

void Plugin::Start()
{
	if (!_loaded || _entryMethod == nullptr || _isolate == nullptr)
		return;

	IScriptFiber* fiber = _isolate->NewFiber();

	fiber->TrySetup(this->_entryMethod);
	fiber->Call(false);
}

void Plugin::Stop()
{
	if (!_loaded || _entryMethod == nullptr || _isolate == nullptr)
		return;

	delete _entryMethod;
	delete _isolate;
}

const char *Plugin::GetName(int* length)
{
	if (length != nullptr)
		*length = this->_name.length();

	return this->_name.c_str();
}



