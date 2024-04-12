// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_SCRIPTCALL_H
#define DUOSCRIPT_SCRIPTCALL_H

class ScriptCall;
class ScriptResult;
class ScriptReturn;

#include "IScript.h"
#include "ScriptFiber.h"

class ScriptReturn : public IScriptReturn
{

};

class ScriptResult : public IScriptResult
{

public:
	enum Strategy
	{
		Invalid,

		//	No strategy, just clean return with returnc
		Clean,

		//	Error on return
		Error,

		//	Yield on return with argc
		Yield,
	};

	Strategy strategy = Invalid;
};

class ScriptCall : public IScriptCall
{

public:
	explicit ScriptCall(ScriptFiber* parent);

public:

	void Setup(int argc);

	ScriptFiber *parent;

	ScriptResult result;

	int argc;
	int returnc;

public:
	virtual IIsolateHandle *GetIsolate();
	virtual IFiberHandle *GetFiber();

	///	Get the first argument (aka "self")
	void* GetOpaqueSelf() override;

	///	Get the number of arguments available
	int GetLength() override;

	///	Get the current namecall, if there is one.
	bool TryGetNamecall(char* result, int maxlen) override;

	bool ArgBool(int slot, bool *result) override;
	bool ArgString(int slot, char* result, int maxlen, int* written) override;
	bool ArgObject(int slot, IScriptRef** result) override;
	bool ArgNumber(int slot, double* result) override;
	bool ArgInt(int slot, int* result) override;
	bool ArgUnsigned(int slot, unsigned* result) override;

	bool ArgMethod(int slot, IScriptMethod** result) override;
	bool ArgTable(int slot, IScriptObject** result) override;
	bool ArgBuffer(int slot, void** result, size_t* size) override;

	void PushBool(bool value) override;
	void PushString(const char* value, int length) override;
	void PushObject(IScriptRef* value) override;
	void PushNumber(double value) override;
	void PushInt(int value) override;
	void PushUnsigned(unsigned value) override;

	void PushVarArgs(IScriptCall* from, int after) override;
	void PushArg(IScriptCall *from, int arg) override;
	void PushBuffer(void* data, size_t length) override;

public:
	///	Invoke an error in the calling method
	virtual IScriptResult* Error(const char* error, ...);

	///	Suspend execution
	virtual IScriptResult* Await();

	///	Return without error
	virtual IScriptResult* Return();
};


#endif //DUOSCRIPT_SCRIPTCALL_H
