// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "AsyncHandle.h"

AsyncHandle::AsyncHandle(ILoop::Async callback, Loop *loop)
{
	_invoked = false;
	_killed = false;
	_parent = loop;
	_callback = callback;

	_handle = new uv_async_t();
	_handle->data = this;

	uv_async_init(&loop->loop, _handle, &AsyncHandle::OnAsync);
}

AsyncHandle::~AsyncHandle()
{
	DuoScope(AsyncHandle::Destroy);

	//	Delete the handle if we haven't already
	if (!_killed && _handle != nullptr)
		uv_close(reinterpret_cast<uv_handle_t *>(_handle), &Loop::OnDeallocate);
}

void AsyncHandle::Kill()
{
	if (_killed || _handle == nullptr)
		return;

	uv_close(reinterpret_cast<uv_handle_t *>(_handle), &Loop::OnDeallocate);
	_handle = nullptr;
	_killed = true;
}

bool AsyncHandle::TryInvoke()
{
	DuoScope(AsyncHandle::TryInvoke);

	if (_killed || _invoked || _handle == nullptr)
		return false;

	//	Schedule invocation on async on next event loop iter
	uv_async_send(_handle);
	_invoked = true;
}

void AsyncHandle::OnAsync(uv_async_t *handle)
{
	DuoScope(AsyncHandle::OnAsync);

	AsyncHandle* self = static_cast<AsyncHandle *>(handle->data);

	if (self->_callback)
		self->_callback(self);

	//	It is the responsibility of the callback to delete
	//	the handle. If they don't, then that's their problem--
	//	we don't want to delete ourselves in case someone is holding
	//	on to our handle for whatever reason.
}


