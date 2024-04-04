// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_ILOOP_H
#define DUOSCRIPT_ILOOP_H

#include <FastDelegate.h>

class IAsyncHandle
{
public:
	virtual ~IAsyncHandle() = default;

	///	Attempt to invoke the callback associated with this async handle.
	///	Returns true when the handle is still alive.
	///	Returns false when the handle has been killed or already invoked.
	virtual bool TryInvoke() = 0;

	///	Kill this handle to prevent it from being used.
	///	This causes all future calls to TryInvoke() to return false.
	virtual void Kill() = 0;
};

class ILoop
{
public:
	///	Async handler
	typedef fastdelegate::FastDelegate<void> Async;

#define ASYNC_HANDLER(method) \
	fastdelegate::MakeDelegate(this, method);

};

#endif //DUOSCRIPT_ILOOP_H
