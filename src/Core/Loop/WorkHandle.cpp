// Copyright (c) 2024 Mooshua. All rights reserved.

#include "WorkHandle.h"

WorkHandle::WorkHandle(void *input, ILoop::WorkProcessor processor, ILoop::WorkReceiver receiver, Loop* parent)
{
	this->_input = input;
	this->_output = nullptr;
	this->_killed = false;
	this->_invoked = false;

	this->_processor = processor;
	this->_receiver = receiver;
	this->_parent = parent;

	this->_handle.data = this;

	uv_queue_work(&parent->loop, &this->_handle, &WorkHandle::OnWork, &WorkHandle::OnFinish);
}

void WorkHandle::OnWork(uv_work_t *request)
{
	WorkHandle* self = static_cast<WorkHandle *>(request->data);

	//	We have already been invoked, so don't try and cancel us now.
	//	Just set _killed in .Kill() and use the finish handler to treat it as a cancel.
	self->_invoked = true;

	self->_output = self->_processor(self->_input);
}

void WorkHandle::OnFinish(uv_work_t *request, int status)
{
	WorkHandle* self = static_cast<WorkHandle *>(request->data);

	if (status != UV_ECANCELED && !self->_killed)
	{
		self->_receiver(self->_input, self->_output);
	}

	delete self;
}

void WorkHandle::Kill()
{
	if (_killed)
		return;

	_killed = true;

	if (!_invoked)
		//	Block the work from being called,
		//	but this does not stop the finalizer from being invoked!
		uv_cancel(reinterpret_cast<uv_req_t *>(&this->_handle));
}

WorkHandle::~WorkHandle()
{
	//	TODO:
}