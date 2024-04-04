// Copyright (c) 2024 Mooshua. All rights reserved.

#include "Loop.h"

Loop g_Loop;

SH_DECL_HOOK3_void(ISource2Server, GameFrame, SH_NOATTRIB, 0, bool, bool, bool);

void Loop::Initialize()
{
	uv_loop_init(&loop);
	uv_mutex_init(&loop_lock);

	active = true;

	SH_ADD_HOOK(ISource2Server, GameFrame, g_Server, SH_MEMBER(this, &Loop::OnGameFrame), false);
}

void Loop::OnGameFrame(bool simulating, bool first, bool last)
{
	if (!active)
		RETURN_META(MRES_IGNORED);

	uv_mutex_lock(&loop_lock);
		if (uv_run(&loop, UV_RUN_NOWAIT) != 0)
		{
			//	TODO: Add a ton of logging and crying here
			active = false;
		}
	uv_mutex_unlock(&loop_lock);

	RETURN_META(MRES_IGNORED);
}

bool Loop::Destroy()
{
	active = false;
	uv_stop(&loop);

	//	Walk the loop to close all handles
	uv_walk(&loop, &Loop::OnWalk, this);

	if (uv_loop_alive(&loop))
		//	Failed shutdown
		return false;

	uv_loop_close(&loop);

	SH_REMOVE_HOOK(ISource2Server, GameFrame, g_Server, SH_MEMBER(this, &Loop::OnGameFrame), false);
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

