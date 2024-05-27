// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#ifndef DUOSCRIPT_ASYNCHANDLE_H
#define DUOSCRIPT_ASYNCHANDLE_H

#include <uv.h>

#include "ILoop.h"
#include "Loop.h"

class AsyncHandle : public virtual IAsyncHandle
{
public:
	AsyncHandle(ILoop::Async handle, Loop* loop);
	~AsyncHandle() override;

	///	Attempt to invoke the callback associated with this async handle.
	///	Returns true when the handle is still alive.
	///	Returns false when the handle has been killed or already invoked.
	bool TryInvoke() override;

	///	Kill this handle to prevent it from being used.
	///	This causes all future calls to TryInvoke() to return false.
	void Kill() override;

	static void OnAsync(uv_async_t *handle);

protected:
	bool _invoked;
	bool _killed;
	uv_async_t *_handle;

	Loop *_parent;
	ILoop::Async _callback;
};


#endif //DUOSCRIPT_ASYNCHANDLE_H
