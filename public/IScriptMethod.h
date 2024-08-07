// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under the Lesser GPL (see public/LICENSE.md), and defines an
// interface that may be linked to without distributing the linkee under a GPL license.

#ifndef DUOSCRIPT_ISCRIPTMETHOD_H
#define DUOSCRIPT_ISCRIPTMETHOD_H

class IScriptMethod;
class IScriptRef;
class IScriptObject;
class IScriptResult;
class IScript;
class IScriptIsolate;
class IScriptCall;
class IScriptPolyglot;
class IBaseScriptControllerEntity;

class IFiberHandle;
class IIsolateHandle;

#include "IScript.h"

#define _ARG_ERROR(call, func, varname, slotnum, what)	 \
	return (call)->Error(                         		 \
		"[%s] Expected argument '%s' (#%d) to be a %s!", \
		func, varname, (slotnum), what 					 \
	)

#define ARG_STRING(call, slot, var)              				  \
    do {                                         				  \
        if (!(call)->ArgString( (slot), &(var) ))  				  \
    		_ARG_ERROR(call, __FUNCTION__, #var, slot, "string"); \
    } while (0)

#define ARG_BUFFER(call, slot, var)              					 \
    do {                                         					 \
        if (!(call)->ArgBuffer( (slot), &(var) ))  					 \
    		_ARG_ERROR(call, __FUNCTION__, #var, slot, "buffer");	 \
    } while (0)

#define ARG_INT(call, slot, var)          	    				 \
    do {                                         				 \
        if (!(call)->ArgInt( (slot), &(var) ))  				 \
    		_ARG_ERROR(call, __FUNCTION__, #var, slot, "int");	 \
    } while (0)

#define ARG_UINT(call, slot, var)          	    					 \
    do {                                         					 \
        if (!(call)->ArgUnsigned( (slot), &(var) ))  				 \
    		_ARG_ERROR(call, __FUNCTION__, #var, slot, "unsigned");	 \
    } while (0)

#define ARG_NUMBER(call, slot, var)          	    				 \
    do {                                         					 \
        if (!(call)->ArgNumber( (slot), &(var) ))  					 \
    		_ARG_ERROR(call, __FUNCTION__, #var, slot, "number");	 \
    } while (0)

#define ARG_BOOL(call, slot, var)  	        	   	 			 \
    do {                                         				 \
        if (!(call)->ArgBool( (slot), &(var) ))  				 \
    		_ARG_ERROR(call, __FUNCTION__, #var, slot, "bool");	 \
    } while (0)


//	I'm not sure if this is the best solution,
//	but it's the one I have now so yee-haw.
///	DO NOT DEPEND--I AM AN IMPLEMENTATION DETAIL.
class IScriptPolyglotView
{
public:
	virtual IScriptPolyglot* ToPolyglot() = 0;
};

class IScriptRead : public virtual IScriptPolyglotView
{
public:
	virtual int GetLength() = 0;

	virtual bool ArgBool(int slot, bool *result) = 0;

	virtual bool ArgObject(int slot, IScriptRef** result) = 0;
	virtual bool ArgNumber(int slot, double* result) = 0;
	virtual bool ArgInt(int slot, int* result) = 0;
	virtual bool ArgUnsigned(int slot, unsigned* result) = 0;

	virtual bool ArgString(int slot, char* result, int maxlen, int* written = nullptr) = 0;
	virtual bool ArgString(int slot, std::string* result) = 0;

	virtual bool ArgBuffer(int slot, void** result, size_t* size) = 0;
	virtual bool ArgBuffer(int slot, std::string* result) = 0;

	//	Get a method passed as an argument.
	//	Note that this can have upvalues and state--so don't send us
	//	across isolate boundaries!
	virtual bool ArgMethod(int slot, IScriptMethod** result) = 0;
	virtual bool ArgTable(int slot, IScriptObject** result) = 0;

	///	@brief Get another controller's entity as an argument
	virtual bool ArgOpaqueEntity(int slot, IBaseScriptControllerEntity** userdata) = 0;

};

class IScriptReturn : public IScriptRead
{
public:
	///	Returns true when the result is a yield
	virtual bool IsYielding() = 0;

	///	Returns true when the thread has errored.
	virtual bool IsError() = 0;

	///	Returns the error, when IsError() is true.
	virtual const char* GetError() = 0;
};

class IScriptInvoke : public virtual IScriptPolyglotView
{
public:
	virtual IIsolateHandle *GetIsolate() = 0;
	virtual IFiberHandle *GetFiber() = 0;

public:
	virtual void PushBool(bool value) = 0;
	virtual void PushString(const char* value, int length = -1) = 0;
	virtual void PushObject(IScriptRef* value) = 0;
	virtual void PushNumber(double value) = 0;
	virtual void PushInt(int value) = 0;
	virtual void PushUnsigned(unsigned value) = 0;

	///	Push all arguments after the specified arguments.
	///	This can be used to pass varargs across C stacks.
	virtual void PushVarArgs(IScriptPolyglotView* from, int after = 1) = 0;
	virtual void PushArg(IScriptPolyglotView *from, int arg) = 0;

	///	Copy a buffer into Lua
	virtual void PushBuffer(void* data, size_t length) = 0;
};

///	@brief Push the provided value to the scriptcall
template<typename Type>
void script_push(IScriptCall* call, Type &&value)
{
	if constexpr ( std::is_arithmetic_v<Type> )
	{
		if constexpr (std::is_floating_point_v<Type>)
			return call->PushNumber(value);
		if (std::is_unsigned_v<Type>)
			return call->PushUnsigned(value);

		return call->PushInt(value);
	}

	if constexpr ( std::is_same_v<Type, bool> )
		return call->PushBool(value);

	if constexpr (std::is_convertible_v<Type, const char*>)
		return call->PushString(value);

	if constexpr (std::is_convertible_v<Type, std::string>)
		return call->PushString((std::string(value)).c_str(), (std::string(value)).size());

	abort();
}

///	Arguments passed to native code from lua
class IScriptCall : public IScriptInvoke, public IScriptRead
{
public:

	///	Get the first argument (aka "self")
	virtual void* GetOpaqueSelf() = 0;

	///	Get the current namecall, if there is one.
	virtual bool TryGetNamecall(char* result, int* atom, int maxlen) = 0;


public:
	///	Generate a response for this call that invokes an error
	///	in the calling method.
	virtual IScriptResult* Error(const char* error, ...) = 0;

	///	Suspend execution
	virtual IScriptResult* Await() = 0;

	virtual IScriptResult* Return() = 0;
public:
	template<typename Entity>
	inline Entity* GetSelf() { return static_cast<Entity*>(this->GetOpaqueSelf()); }
};

///	This is a helper object that wraps the virtual tables
///	for the internally used "polyglot" object.
///	DO NOT DEPEND--I AM AN IMPLEMENTATION DETAIL.
///	See above "IScriptPolyglotView"
class IScriptPolyglot : public IScriptCall, public IScriptReturn
{

};

#endif //DUOSCRIPT_ISCRIPTMETHOD_H
