// Copyright (c) 2024 Mooshua. All rights reserved.

#include "LuauFrontend.h"
#include "Luau/Config.h"
#include "Luau/LinterConfig.h"
#include "Luau/Transpiler.h"
#include "LogicGlobals.h"
#include "ILogger.h"
#include "LuauCheckResult.h"
#include "Luau/BuiltinDefinitions.h"

LuauFrontendController g_LuauFrontend;

LuauFileResolver::LuauFileResolver(IIsolateHandle *isolate, IScriptMethod *readmethod, IScriptMethod* resolvemethod)
{
	this->isolate = isolate;
	this->readmethod = readmethod;
	this->resolvemethod = resolvemethod;
}

LuauFileResolver::~LuauFileResolver()
{
	delete this->readmethod;
	delete this->isolate;
}

std::optional<Luau::SourceCode> LuauFileResolver::readSource(const Luau::ModuleName &name)
{
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
		g_DuoLog->Message("FileResolver", ILogger::SEV_WARN,
						  "nil received when reading file %s", name.c_str());
		return std::nullopt;
	}

	delete fiber;
	return sourceCode;
}

std::optional<Luau::ModuleInfo> LuauFileResolver::resolveModule(const Luau::ModuleInfo *context, Luau::AstExpr *expr)
{
	IScriptFiber* fiber = this->isolate->Get()->NewFiber();
	IScriptInvoke* args;
	fiber->TrySetup(this->resolvemethod, &args);
	{
		//	Constant string require() invocation?
		if (Luau::AstExprConstantString* string = expr->as<Luau::AstExprConstantString>())
			args->PushString(string->value.begin());

		//	Others: Wtf do we do here?
	}
	IScriptReturn* results = fiber->Call(true);

	Luau::ModuleInfo module;

	if (!results->ArgString(1, &module.name)) {
		g_DuoLog->Message("FileResolver", ILogger::SEV_WARN,
						  "nil received when looking up %s", Luau::toString(expr).c_str());
		return std::nullopt;
	}
	delete fiber;
	return module;
}

IScriptResult *LuauFrontendController::New(IScriptCall *call)
{
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
	if (!call->ArgMethod(1, &fileResolve))
		return call->Error("Missing FileResolver in argument 1!");
	if (!call->ArgMethod(2, &moduleResolve))
		return call->Error("Missing ModuleResolver in argument 2!");

	LuauFileResolver* fileResolver = new LuauFileResolver(call->GetIsolate(), fileResolve, moduleResolve);

	entity.Build(fileResolver, configResolver);

	Luau::registerBuiltinGlobals(*entity.frontend, entity.frontend->globals);

	return this->ReturnNew(call, &entity);
}

IScriptResult *LuauFrontendController::Check(LuauFrontend *entity, IScriptCall *call)
{
	std::string name = std::string();
	if (!call->ArgString(1, &name))
		return call->Error("Expected argument 1 to be a string!");

	Luau::CheckResult result = entity->frontend->check(name);
	LuauCheckResult resultEntity;
	resultEntity.result = result;

	return g_LuauCheckResult.ReturnNew(call, &resultEntity);
}
