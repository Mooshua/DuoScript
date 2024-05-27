// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#ifndef DUOSCRIPT_WORKHANDLE_H
#define DUOSCRIPT_WORKHANDLE_H

#include <uv.h>
#include <ILoop.h>

#include "Loop.h"

class WorkHandle : public virtual IBaseHandle
{
public:
	WorkHandle(void* input, ILoop::WorkProcessor processor, ILoop::WorkReceiver receiver, Loop* parent);
	~WorkHandle() override;

	void Kill() override;

	static void OnWork(uv_work_t *request);
	static void OnFinish(uv_work_t *request, int status);

protected:
	bool _killed;
	bool _invoked;

	uv_work_t _handle;
	Loop *_parent;

	void* _input;
	void* _output;

	ILoop::WorkProcessor _processor;
	ILoop::WorkReceiver _receiver;
};


#endif //DUOSCRIPT_WORKHANDLE_H
