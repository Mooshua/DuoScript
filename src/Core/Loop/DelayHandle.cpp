// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "DelayHandle.h"

DelayHandle::DelayHandle(ILoop::Delay callback, Loop *loop)
{
	_invoked = false;
	_killed = false;
	_parent = loop;
	_callback = callback;

	_handle = new uv_timer_t();
	_handle->data = this;

	uv_timer_init(&loop->loop, _handle);
}

DelayHandle::~DelayHandle()
{
	if (!_killed && _handle != nullptr)
		uv_close(reinterpret_cast<uv_handle_t *>(_handle), &Loop::OnDeallocate);
}

void DelayHandle::Kill()
{
	if (!_killed && _handle != nullptr)
		uv_close(reinterpret_cast<uv_handle_t *>(_handle), &Loop::OnDeallocate);

	_killed = true;
	_handle = nullptr;
}

bool DelayHandle::TryDelay(uint64_t milliseconds)
{
	if (_killed || _handle == nullptr)
		return false;

	if (_invoked)
		uv_timer_stop(_handle);

	//	Schedule invocation ONCE after timeout.
	uv_timer_start(_handle, &DelayHandle::OnDelay, milliseconds, 0);

	_invoked = true;
}

uint64_t DelayHandle::Remaining()
{
	if (_killed || _handle == nullptr)
		return -1;

	return uv_timer_get_due_in(_handle);
}

void DelayHandle::OnDelay(uv_timer_t *handle)
{
	DelayHandle* self = static_cast<DelayHandle *>(handle->data);

	if (self->_callback)
		self->_callback(self);

	//	It is the responsibility of the callback to delete
	//	the handle. If they don't, then that's their problem--
	//	we don't want to delete ourselves in case someone is holding
	//	on to our handle for whatever reason.
}
