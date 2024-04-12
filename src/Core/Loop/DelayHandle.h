// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_DELAYHANDLE_H
#define DUOSCRIPT_DELAYHANDLE_H

#include "Loop.h"
#include "ILoop.h"

class DelayHandle : public virtual IDelayHandle
{
public:
	DelayHandle(ILoop::Delay callback, Loop* loop);
	~DelayHandle() override;

	///	Kill this handle to prevent it from being used.
	///	This causes all future invocation attempts to fail.
	virtual void Kill();

	///	Attempt to invoke this delay.
	///	If we have already invoked the delay but
	///	the callback has not been handled yet, then we
	///	will cancel the in-flight request and send this one.
	virtual bool TryDelay(uint64_t milliseconds);

	///	Get the time remaining until the callback is invoked,
	///	in milliseconds.
	virtual uint64_t Remaining();

	static void OnDelay(uv_timer_t *handle);


protected:

	bool _invoked;
	bool _killed;
	uv_timer_t *_handle;

	Loop *_parent;
	ILoop::Delay _callback;
};


#endif //DUOSCRIPT_DELAYHANDLE_H
