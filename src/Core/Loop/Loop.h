// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_LOOP_H
#define DUOSCRIPT_LOOP_H

#include "uv.h"

#include "ILoop.h"

class Loop : public ILoop
{
public:
	///	Initialize libuv
	void Initialize();

	bool Destroy();

public:
	uv_loop_t* AsLoop() override;
	IAsyncHandle* NewAsync(ILoop::Async callback) override;
	IDelayHandle* NewDelay(ILoop::Delay callback) override;
	IBaseHandle* NewWorkUnsafe(void* work, WorkProcessor processor, WorkReceiver receiver) override;

public:
	//	Run one frame
	void RunOnce();
private:
	static void OnAsync(uv_async_t *handle);

	///	Destroy all handles in libuv to perform a clean shutdown
	static void OnWalk(uv_handle_t *handle, void* userdata);

public:
	///	Callback for when a handle is closed.
	static void OnDeallocate(uv_handle_t* handle);

public:
	uv_loop_t loop;
private:
	bool active;

	///	Lock that guards changes to Loop::loop.
	uv_mutex_t loop_lock;
};

extern Loop g_Loop;

#endif //DUOSCRIPT_LOOP_H
