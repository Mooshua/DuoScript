// Copyright (c) 2024 Mooshua. All rights reserved.

#define NOT_COMPILING_DUOSCRIPT

#include "Script/ScriptVM.h"
#include "TestController.h"
#include "Luau/Compiler.h"
#include "Script/ScriptFiber.h"
#include "uv.h"

TestController g_TestController;

int main()
{
	(&g_ScriptVM)->Initialize();
	IScriptControllerManager* manager = g_ScriptVM.controllers;

	manager->Register(&g_TestController);

	Luau::CompileOptions options {};

	options.optimizationLevel = 2;
	options.debugLevel = 2;

	std::string code = Luau::compile(R"HI(
		local function bench()
			local object = Test:New()
			--[[print(object)
			print(object:GetCount())
			print(object:Add(), object:GetCount())
			print(object:Add(5), object:GetCount())]]

			local total = 0;

			for i = 0, 10000000 do
				object:Add();
				total += object:GetCount()
			end

			print("Total", total);
		end bench()
	)HI", options);

	auto isolate = (&g_ScriptVM)->CreateIsolate();
	auto fiber = isolate->NewFiber();

	IScriptMethod* method;
	if (!isolate->TryLoad("Test", code.data(), code.length(), &method, nullptr, 0))
	{
		printf("Fail");
		return -1;
	}

	fiber->TrySetup(method);

	uint64_t time = uv_hrtime();

	fiber->Call();

	uint64_t after = uv_hrtime();
	printf("%llu\n", after - time);
	printf("%llu\n", (after - time) / 10000000);

	return 0;
}
