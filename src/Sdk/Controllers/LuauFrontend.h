// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#ifndef DUOSCRIPT_LUAUFRONTEND_H
#define DUOSCRIPT_LUAUFRONTEND_H

#include <Generic/StringMap.h>
#include <ILogger.h>
#include <IScriptController.h>
#include "Luau/FileResolver.h"
#include "Luau/Config.h"
#include "Luau/Frontend.h"

class LuauFrontend
{
public:
	void Build(Luau::FileResolver* fileResolver, Luau::ConfigResolver* configResolver)
	{
		this->frontend = std::make_shared<Luau::Frontend>
		        (fileResolver, configResolver, this->options);
	}

	LuauFrontend& operator=(const LuauFrontend& other)
	{
		//	Welcome to another episode of "C++ sucks"
		auto bullshit = std::shared_ptr<Luau::FrontendCancellationToken>();
		memcpy(&this->options.cancellationToken, &bullshit, sizeof(std::shared_ptr<Luau::FrontendCancellationToken>));

		this->options = other.options;
		this->frontend = other.frontend;
		return *this;
	}
public:
	Luau::FrontendOptions options;
	std::shared_ptr<Luau::Frontend> frontend;
};

class LuauConfigResolver : public Luau::ConfigResolver
{
public:
	LuauConfigResolver(Luau::Config config)
	: config(config)
	{

	}
	Luau::Config config;
	virtual const Luau::Config& getConfig(const Luau::ModuleName& name) const override
	{
		return config;
	}
};

class LuauFileResolver : public Luau::FileResolver
{
	ILogger* _log;
	std::shared_ptr<Luau::Frontend> _frontend;
	ke::StringMap<void*>* _environments;

public:
	LuauFileResolver(ILogger* log, IIsolateHandle* isolate, IScriptMethod* readmethod, IScriptMethod* resolveMethod, IScriptMethod* envmethod);
	~LuauFileResolver() override;

	void PostBuild(std::shared_ptr<Luau::Frontend> frontend)
	{
		_frontend = frontend;
	}

public:	//	FileResolver
	std::optional<Luau::SourceCode> readSource(const Luau::ModuleName& name) override;
	std::optional<Luau::ModuleInfo> resolveModule(const Luau::ModuleInfo* context, Luau::AstExpr* expr) override;
	std::optional<std::string> getEnvironmentForModule(const Luau::ModuleName& name) const override;

protected:
	IScriptMethod* readmethod;
	IScriptMethod* resolvemethod;
	IScriptMethod* envmethod;

	IIsolateHandle* isolate;
};

class LuauFrontendController : public IScriptController<LuauFrontend>
{
	ILogger* _log;
public:
	LuauFrontendController(ILogger* log)
	: _log(log)
	{
		CONTROLLER_NAME(LuauFrontend);

		CONTROLLER_STATIC_METHOD(New, &LuauFrontendController::New);

		CONTROLLER_METHOD(Check, &LuauFrontendController::Check);
	}

public:
	IScriptResult* New(IScriptCall* call);

public:
	IScriptResult* Check(LuauFrontend* entity, IScriptCall* call);
	IScriptResult* AddType(LuauFrontend* entity, IScriptCall* call);

};


#endif //DUOSCRIPT_LUAUFRONTEND_H
