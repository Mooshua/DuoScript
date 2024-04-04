// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_LOOP_H
#define DUOSCRIPT_LOOP_H

#include <DuoScript.h>

#include <ILoop.h>
#include <uv.h>

class Loop : public ILoop
{
public:
	///	Initialize libuv
	void Initialize();
	bool Destroy();

private:
	void OnGameFrame(bool simulating, bool first, bool last);
	static void OnAsync(uv_async_t *handle);

	///	Destroy all handles in libuv to perform a clean shutdown
	static void OnWalk(uv_handle_t *handle, void* userdata);
	static void OnDeallocate(uv_handle_t* handle);
private:
	bool active;

	uv_loop_t loop;
	///	Lock that guards changes to Loop::loop.
	uv_mutex_t loop_lock;
};

extern Loop g_Loop;

#endif //DUOSCRIPT_LOOP_H
