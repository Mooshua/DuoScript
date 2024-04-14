// Copyright (c) 2024 Mooshua. All rights reserved.

#define NOT_COMPILING_DUOSCRIPT

#include "uv.h"
#include <iostream>

#include "Script/ScriptVM.h"
#include "TestController.h"
#include "Luau/Compiler.h"
#include "Script/ScriptFiber.h"

TestController g_TestController;

uv_idle_t g_Idle;

void idle(uv_idle_t* idle)
{
	//printf(".");
}

int main()
{
	uv_loop_init(&g_Loop.loop);

	(&g_ScriptVM)->Initialize();
	IScriptControllerManager* manager = g_ScriptVM.controllers;

	manager->Register(&g_TestController);

	Luau::CompileOptions options {};

	options.optimizationLevel = 2;
	options.debugLevel = 2;

	std::string code = Luau::compile(R"HI(
		print("[*] Opening")
		local _, file = File:Open("hi.txt");
		print("[*] Reading", file)
		local a, contents = file:Read(10);
		print("[*] Read!",a, contents)

		local b, contents2 = file:Read(10);
		print("[*] Read!",b, contents2)

		local success = file:Write(buffer.fromstring("\nomg!!!"));

		print("[*] Wrote!", success)

		print(buffer.tostring(contents));
		print(buffer.tostring(contents2));

		--[==[local function bench()
			local object = Test:New()
			--[[print(object)
			print(object:GetCount())
			print(object:Add(), object:GetCount())
			print(object:Add(5), object:GetCount())]]

			local total = 0;

			for i = 0, 10000000 do
				object:Add();
			end

			print("Total", total);
		end bench()]==]
	)HI", options);

	auto isolate = (&g_ScriptVM)->CreateIsolate();
	auto fiber = isolate->NewFiber();

	IScriptMethod* method;
	if (!isolate->TryLoad("Test", code, &method, nullptr, 0))
	{
		printf("Fail");
		return -1;
	}

	if (!fiber->TrySetup(method))
		printf("Fail Setup");

	fiber->Call(false);

	uv_idle_init(&g_Loop.loop, &g_Idle);
	uv_idle_start(&g_Idle, &idle);

	uv_run(&g_Loop.loop, UV_RUN_DEFAULT);

	/*
	uint64_t time = uv_hrtime();

	fiber->Call();

	uint64_t after = uv_hrtime();
	printf("%llu\n", after - time);
	printf("%llu\n", (after - time) / 10000000);

*/


	return 0;
}
