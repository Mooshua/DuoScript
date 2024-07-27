// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "LuauFrontend.h"
#include "Luau/Config.h"
#include "Luau/LinterConfig.h"
#include "Luau/Transpiler.h"
#include "Luau/Frontend.h"
#include "ILogger.h"
#include "LuauCheckResult.h"
#include "Luau/BuiltinDefinitions.h"

#include <numeric>


LuauFileResolver::LuauFileResolver(
		ILogger* logger,
		IIsolateHandle *isolate,
								   IScriptMethod* readmethod,
								   IScriptMethod* resolvemethod,
								   IScriptMethod* envmethod
								   )
{
	this->_log = logger;
	this->isolate = isolate;
	this->readmethod = readmethod;
	this->resolvemethod = resolvemethod;
	this->envmethod = envmethod;
}

LuauFileResolver::~LuauFileResolver()
{
	delete this->readmethod;
	delete this->resolvemethod;
	delete this->envmethod;

	delete this->isolate;
}

std::optional<Luau::SourceCode> LuauFileResolver::readSource(const Luau::ModuleName &name)
{
	DuoScope(LuauFrontend::ReadSource);

	IScriptFiber* fiber = this->isolate->Get()->NewFiber();
	IScriptInvoke* args;
	fiber->TrySetup(this->readmethod, &args);
	{
		args->PushString(name.c_str());
	}
	IScriptReturn* results = fiber->Call(true);

	Luau::SourceCode sourceCode;

	//	Note: internal constraint inherited from Roblox codebase
	//	all require()able scripts must be a module, or Luau will
	//	complain thinking it's not a ModuleScript :^)
	sourceCode.type = Luau::SourceCode::Module;

	if (!results->ArgString(1, &sourceCode.source)) {
		_log->Message("FileResolver", ILogger::SEV_WARN,
						  "nil received when reading file %s", name.c_str());
		return std::nullopt;
	}

	delete fiber;
	return sourceCode;
}

std::optional<Luau::ModuleInfo> LuauFileResolver::resolveModule(const Luau::ModuleInfo *context, Luau::AstExpr *expr)
{
	DuoScope(LuauFrontend::ResolveModule);

	IScriptFiber* fiber = this->isolate->Get()->NewFiber();
	IScriptInvoke* args;
	fiber->TrySetup(this->resolvemethod, &args);
	{
		//	Are we using fancy schmancy indexing?
		auto segments = Luau::parsePathExpr(*expr);
		if (segments.size() == 0)
		{
			//	No? Constant string require() invocation?
			if (Luau::AstExprConstantString* string = expr->as<Luau::AstExprConstantString>())
				args->PushString(string->value.begin());
		}
		else
		{
			//	Segments! Join 'em
			std::string ret;
			for(const auto &s : segments) {
				if(!ret.empty())
					ret += ".";
				ret += s;
			}

			args->PushString(ret.c_str());
		}

		//	Others: Wtf do we do here?
	}
	IScriptReturn* results = fiber->Call(true);

	Luau::ModuleInfo module;

	if (!results->ArgString(1, &module.name)) {
		_log->Message("FileResolver", ILogger::SEV_WARN,
						  "nil received when looking up %s", Luau::toString(expr).c_str());
		return std::nullopt;
	}
	delete fiber;
	return module;
}

std::optional<std::string> LuauFileResolver::getEnvironmentForModule(const Luau::ModuleName &name) const
{
	DuoScope(LuauFrontend::ResolveEnvironment);

	IScriptFiber* fiber = this->isolate->Get()->NewFiber();
	IScriptInvoke* args;
	fiber->TrySetup(this->envmethod, &args);
	{
		args->PushString(name.c_str());
	}
	IScriptReturn* results = fiber->Call(true);

	std::string environment;

	if (!results->ArgString(1, &environment)) {
		_log->Message("FileResolver", ILogger::SEV_WARN,
						  "nil received when getting environment of module %s", name.c_str());
		return std::nullopt;
	}

	delete fiber;
	return environment;
}

IScriptResult *LuauFrontendController::New(IScriptCall *call)
{
	DuoScope(LuauFrontend::New);

	LuauFrontend entity;
	Luau::Config config;

	//	Enable the inference engine.
	config.mode = Luau::Mode::Strict;
	config.parseOptions.allowDeclarationSyntax = true;
	config.parseOptions.captureComments = true;
	config.enabledLint.setDefaults();

	//	Some lints are very useful, as they virtually always point to bugs
	//	We will error on these since people should NOT be writing code like this.
	config.fatalLint.enableWarning(Luau::LintWarning::Code_FormatString);
	config.fatalLint.enableWarning(Luau::LintWarning::Code_MisleadingAndOr);
	config.fatalLint.enableWarning(Luau::LintWarning::Code_UnknownType);
	config.fatalLint.enableWarning(Luau::LintWarning::Code_ForRange);
	config.fatalLint.enableWarning(Luau::LintWarning::Code_ComparisonPrecedence);

	entity.options.cancellationToken = std::shared_ptr<Luau::FrontendCancellationToken>();
	entity.options.retainFullTypeGraphs = true;
	entity.options.runLintChecks = true;
	entity.options.moduleTimeLimitSec = 600;	//	ten minutes. oh, the hell i will raise if this needs to be upped.
	entity.options.enabledLintWarnings = config.enabledLint;

	LuauConfigResolver* configResolver = new LuauConfigResolver(config);

	IScriptMethod* fileResolve;
	IScriptMethod* moduleResolve;
	IScriptMethod* envResolve;
	if (!call->ArgMethod(1, &fileResolve))
		return call->Error("Missing FileResolver in argument 1!");
	if (!call->ArgMethod(2, &moduleResolve))
		return call->Error("Missing ModuleResolver in argument 2!");
	if (!call->ArgMethod(2, &envResolve))
		return call->Error("Missing EnvironmentResolver in argument 3!");

	LuauFileResolver* fileResolver = new LuauFileResolver(_log, call->GetIsolate(), fileResolve, moduleResolve, envResolve);

	entity.Build(fileResolver, configResolver);

	Luau::registerBuiltinGlobals(*entity.frontend, entity.frontend->globals);

	return this->ReturnNew(call, &entity);
}

IScriptResult *LuauFrontendController::Check(LuauFrontend *entity, IScriptCall *call)
{
	DuoScope(LuauFrontend::Check);

	std::string name = std::string();
	if (!call->ArgString(1, &name))
		return call->Error("Expected argument 1 to be a string!");

	Luau::CheckResult result = entity->frontend->check(name);
	LuauCheckResult resultEntity;
	resultEntity.result = result;

	return g_LuauCheckResult.ReturnNew(call, &resultEntity);
}

IScriptResult *LuauFrontendController::AddType(LuauFrontend *entity, IScriptCall *call)
{
	DuoScope(LuauFrontend::AddDefines);

	//	Package name used for documentation
	std::string name;
	if (!call->ArgString(1, &name))
		return call->Error("Expected argument 1 to be a string 'name'!");

	std::string definition;
	if (!call->ArgString(2, &definition))
		return call->Error("Expected argument 2 to be a string 'definitions'!");

	//	Should this go in a global scope or a module scope?
	std::string environment;
	Luau::ScopePtr environment_scope;
	if (call->ArgString(3, &environment))
		environment_scope = entity->frontend->getEnvironmentScope(environment);
	else
		environment_scope = entity->frontend->globals.globalScope;

	entity->frontend->loadDefinitionFile(
			entity->frontend->globals,
			environment_scope,
			definition,
			name,
			true,	//	capture comments?
			false // check for autocomplete?
			);

	return call->Return();
}
