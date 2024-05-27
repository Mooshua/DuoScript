// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_LUAUFRONTEND_H
#define DUOSCRIPT_LUAUFRONTEND_H

#include <IScriptController.h>
#include "Luau/FileResolver.h"
#include "Luau/Config.h"
#include "Luau/Frontend.h"

class LuauFrontend
{
public:
	void Build(Luau::FileResolver* fileResolver, Luau::ConfigResolver* configResolver)
	{
		this->frontend = new Luau::Frontend(fileResolver, configResolver, this->options);
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
	Luau::Frontend* frontend;
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
public:
	LuauFileResolver(IIsolateHandle* isolate, IScriptMethod* readmethod, IScriptMethod* resolveMethod);
	~LuauFileResolver() override;

public:	//	FileResolver
	std::optional<Luau::SourceCode> readSource(const Luau::ModuleName& name) override;
	std::optional<Luau::ModuleInfo> resolveModule(const Luau::ModuleInfo* context, Luau::AstExpr* expr) override;

protected:
	IScriptMethod* readmethod;
	IScriptMethod* resolvemethod;

	IIsolateHandle* isolate;
};

class LuauFrontendController : public IScriptController<LuauFrontend>
{
public:
	LuauFrontendController()
	{
		CONTROLLER_NAME(LuauFrontend);

		CONTROLLER_STATIC_METHOD(New, &LuauFrontendController::New);

		CONTROLLER_METHOD(Check, &LuauFrontendController::Check);
	}

public:
	IScriptResult* New(IScriptCall* call);

public:
	IScriptResult* Check(LuauFrontend* entity, IScriptCall* call);
};

extern LuauFrontendController g_LuauFrontend;

#endif //DUOSCRIPT_LUAUFRONTEND_H
