// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_WORKHANDLE_H
#define DUOSCRIPT_WORKHANDLE_H

#include <ILoop.h>
#include <uv.h>

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
