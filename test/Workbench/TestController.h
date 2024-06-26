// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_TESTCONTROLLER_H
#define DUOSCRIPT_TESTCONTROLLER_H


#include "IScriptController.h"
#include "Loop/Loop.h"

class TestEntity
{
public:
	int count = 0;
};

class TestController : public virtual IScriptController<TestEntity>
{
public:
	TestController()
	{
		CONTROLLER_NAME(Test);

		CONTROLLER_STATIC_METHOD(Hello, &TestController::Hello);
		CONTROLLER_STATIC_METHOD(New, &TestController::New);
		CONTROLLER_STATIC_METHOD(Await, &TestController::Await);

		CONTROLLER_GETTER(Count, &TestController::GetCount);

		CONTROLLER_METHOD(GetCount, &TestController::GetCount);
		CONTROLLER_METHOD(Add, &TestController::Add);
	}

	IScriptResult* New(IScriptCall* call)
	{
		TestEntity entity = TestEntity();

		return this->ReturnNew(call, &entity);
	}

	IScriptResult* Await(IScriptCall* call)
	{
		printf("Yielding\n");

		return call->Await();
	}

	IScriptResult* GetCount(TestEntity *self, IScriptCall *call)
	{
		call->PushInt(self->count);
		return call->Return();
	}

	IScriptResult* Add(TestEntity *self, IScriptCall *call)
	{
		IFiberHandle* a;

		int amount = 1;
		call->ArgInt(1, &amount);

		self->count += amount;
		return call->Return();
	}

	IScriptResult* Hello(IScriptCall* call)
	{
		call->PushInt(1234);
		return call->Return();
	}
};


#endif //DUOSCRIPT_TESTCONTROLLER_H
