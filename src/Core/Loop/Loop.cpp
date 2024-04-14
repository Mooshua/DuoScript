// Copyright (c) 2024 Mooshua. All rights reserved.

#include <DuoMetamod.h>

#include "Loop.h"
#include "AsyncHandle.h"
#include "DelayHandle.h"
#include "Logging/Log.h"
#include "WorkHandle.h"

Loop g_Loop;


void Loop::Initialize()
{
	uv_loop_init(&loop);
	uv_mutex_init(&loop_lock);

	active = true;


	g_Log.Component("Loop", Log::STAT_GOOD, "Started");
}

void Loop::RunOnce()
{
	if (!active)
		return;

	/*if (first)
		g_Log.Message("Loop", Log::SEV_DEBUG, "First Tick");

	if (last)
		g_Log.Message("Loop", Log::SEV_DEBUG, "Last Tick");*/

	uv_mutex_lock(&loop_lock);
		uv_run(&loop, UV_RUN_NOWAIT);
	uv_mutex_unlock(&loop_lock);
}

bool Loop::Destroy()
{
	active = false;
	uv_stop(&loop);

	uv_run(&loop, UV_RUN_NOWAIT);

	//	Walk the loop to close all handles
	uv_walk(&loop, &Loop::OnWalk, this);

	//	Give the loop one iteration to invoke UVCANCELLED callbacks.
	//	Without this, the loop will not unload.
	uv_run(&loop, UV_RUN_NOWAIT);

	if (uv_loop_alive(&loop))
		//	Failed shutdown
		return false;

	uv_loop_close(&loop);

	g_Log.Component("Loop", Log::STAT_NOTE, "Stopped");

	return true;
}

void Loop::OnWalk(uv_handle_t *handle, void *userdata)
{
	uv_close(handle, &Loop::OnDeallocate);
}

void Loop::OnDeallocate(uv_handle_t *handle)
{
	if (handle != nullptr)
		delete handle;
}

IAsyncHandle *Loop::NewAsync(ILoop::Async callback)
{
	return new AsyncHandle(callback, this);
}

IDelayHandle *Loop::NewDelay(ILoop::Delay callback)
{
	return new DelayHandle(callback, this);
}

IBaseHandle *Loop::NewWorkUnsafe(void *work, ILoop::WorkProcessor processor, ILoop::WorkReceiver receiver)
{
	return new WorkHandle(work, processor, receiver, this);
}

uv_loop_t *Loop::AsLoop()
{
	return &this->loop;
}

