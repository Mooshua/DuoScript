// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_ILOOP_H
#define DUOSCRIPT_ILOOP_H

#include <FastDelegate.h>
#include <cstdint>

//	Declare this here so we don't have to
//	reference uv.h. Enables practical referencing
//	to our loop without libuv linkage.
//	Is this causing errors? Put #include <uv.h> ABOVE <ILoop.h>!
#ifndef UV_H
	struct uv_loop_t;
#endif

class IBaseHandle
{
public:
	virtual ~IBaseHandle() = default;

	///	Kill this handle to prevent it from being used.
	///	This causes all future invocation attempts to fail.
	virtual void Kill() = 0;
};

class IAsyncHandle : public virtual IBaseHandle
{
public:
	///	Attempt to invoke the callback associated with this async handle.
	///	Returns true when the handle is still alive.
	///	Returns false when the handle has been killed or already invoked.
	virtual bool TryInvoke() = 0;
};

class IDelayHandle : public virtual IBaseHandle
{
public:
	///	Attempt to invoke this delay.
	///	If we have already invoked the delay but
	///	the callback has not been handled yet, then we
	///	will cancel the in-flight request and send this one.
	virtual bool TryDelay(uint64_t milliseconds) = 0;

	///	Get the time remaining until the callback is invoked,
	///	in milliseconds.
	virtual uint64_t Remaining() = 0;
};

class ITask
{
public:
	fastdelegate::FastDelegate<void*, void*> _processor =
		fastdelegate::MakeDelegate(this, &ITask::OnExecute);

	fastdelegate::FastDelegate<void, void*, void*> _finalizer =
			fastdelegate::MakeDelegate(this, &ITask::OnFinalize);

	virtual ~ITask() = default;
protected:
	void* OnExecute(void* input)
	{
		this->Execute();
		return nullptr;
	}

	void OnFinalize(void* input, void* output)
	{
		this->Finish();
		delete this;
	}

protected:
	virtual void Execute() = 0;
	virtual void Finish() = 0;
};

class ILoop
{
public:
	///	Async handler delegate
	typedef fastdelegate::FastDelegate<void, IAsyncHandle*> Async;

	///	Delay handler delegate
	typedef fastdelegate::FastDelegate<void, IDelayHandle*> Delay;

	typedef fastdelegate::FastDelegate<void*, void*> WorkProcessor;
	typedef fastdelegate::FastDelegate<void, void*, void*> WorkReceiver;

#define LOOP_HANDLER(this, method) \
	fastdelegate::MakeDelegate(this, method);


public:
	virtual uv_loop_t* AsLoop() = 0;

	virtual IAsyncHandle* NewAsync(ILoop::Async callback) = 0;
	virtual IDelayHandle* NewDelay(ILoop::Delay callback) = 0;

	///	Schedule a work item to execute on another thread, off of the game thread.
	///	!! THIS IS UNSAFE !! - Once execution of the receiver has finished,
	///	this handle will be freed! Avoid using unless you really need it.
	///	If emulate is true--The work will be ran synchronously, but it will be emulated
	///	as async.
	virtual IBaseHandle* NewWorkUnsafe(void* work, WorkProcessor processor, WorkReceiver receiver, bool emulate) = 0;

	///	Schedule a work item to execute on another thread, off of the game thread.
	void NewWork(void* work, WorkProcessor processor, WorkReceiver receiver, bool emulate = false)
	{
		this->NewWorkUnsafe(work, processor, receiver, emulate);
	}

	///	Schedule a work item to execute on another thread, off of the game thread.
	///	The receiver will run on the main thread at a script-safe time.
	template<typename Input, typename Output>
	inline void NewTypedWork(
		Input* work,
		fastdelegate::FastDelegate<Output*, IBaseHandle*, Input*> processor,
		fastdelegate::FastDelegate<void, IBaseHandle*, Input*, Output*> receiver,
		bool emulate = false)
	{
		return this->NewWork(work, processor, receiver, emulate);
	}

	inline void NewTask(ITask* task, bool emulate = false)
	{
		return this->NewWork(nullptr, task->_processor, task->_finalizer, emulate);
	}
};

#endif //DUOSCRIPT_ILOOP_H
