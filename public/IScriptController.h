// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_ISCRIPTCONTROLLER_H
#define DUOSCRIPT_ISCRIPTCONTROLLER_H

#include <map>

#include "Generic/StringMap.h"
#include "Generic/StringLiteral.h"
#include "IScript.h"

class IBaseScriptController;
class IBaseScriptControllerEntity;

template<typename Entity>
class IScriptController;

class IControllerInstance
{
public:
	virtual ~IControllerInstance()
	= default;

	template<class Entity>
	IScriptRef* Make(IScriptIsolate* vm, Entity** result = nullptr)
	{
		return this->Make(vm, (void**)result, sizeof(IScriptControllerEntity<Entity>));
	}

public:
	//	Implementation details: Do not use
	virtual IScriptRef* GetMetatable() = 0;
	virtual IScriptRef* GetStaticMetatable() = 0;

protected:
	virtual IScriptRef* Make(IScriptIsolate* vm, void** result, int size) = 0;
};

class IBaseScriptController
{
public:
	IBaseScriptController()
	{

	}

	friend class IScript;
	friend class IScriptControllerManager;
public:

	///	Set the entity template used to construct items of this
	///	entity type.
	void SetTemplate(IControllerInstance* entityTemplate)
	{
		_template = entityTemplate;
	}

	///	Get the name of this controller.
	virtual const char* GetName() = 0;

	virtual IScriptResult* Namecall(IScriptCall* args) = 0;
	virtual IScriptResult* NamecallStatic(IScriptCall* args) = 0;

	virtual void DestroyEntity(void* entity) = 0;

	///	An opaque pointer to our namecall, allowing
	///	the script engine to invoke our namecall without
	///	actually needing to know about us.
	fastdelegate::FastDelegate<IScriptResult*, IScriptCall*> _namecall
		= fastdelegate::MakeDelegate(this, &IBaseScriptController::Namecall);

	fastdelegate::FastDelegate<IScriptResult*, IScriptCall*> _namecallStatic
		= fastdelegate::MakeDelegate(this, &IBaseScriptController::NamecallStatic);

#if 0
	fastdelegate::FastDelegate<void, void*> _destructor
		= fastdelegate::MakeDelegate(this, &IBaseScriptController::DestroyEntity);
#endif

protected:
	IControllerInstance* _template;
};

class IBaseScriptControllerEntity
{
public:
	virtual IBaseScriptController* GetController() = 0;
	virtual void* GetEntity() = 0;
};

template<typename Entity>
class IScriptControllerEntity : IBaseScriptControllerEntity
{
public:
	IScriptControllerEntity(IBaseScriptController* controller)
		: _controller(controller)
	{}

	IBaseScriptController* GetController() final { return _controller; }
	void* GetEntity() final { return &_entity; }

private:
	IBaseScriptController* _controller;
	Entity _entity;
};


///	A script controller handles requests for
///	entities in the script world. Entities hold the state,
///	while controllers process requests for the entity.
template<typename Entity>
class IScriptController : public virtual IBaseScriptController
{

	friend class IScript;
	friend class IScriptControllerManager;
public:
	typedef fastdelegate::FastDelegate<IScriptResult*, Entity*, IScriptCall*> MethodCallback;
	typedef fastdelegate::FastDelegate<IScriptResult*, IScriptCall*> StaticMethodCallback;

	///	The type that will be instantiated in the script VM
	typedef IScriptControllerEntity<Entity> Userdata;

	IScriptController()
	{
		_entityCallbacks.init();
		_staticCallbacks.init();
	}

protected:



	///	Return to an IScriptCall with a new entity
	IScriptResult* ReturnNew(IScriptCall* call, Entity* value)
	{
		Entity* userdata;
		IScriptRef* entity = _template->Make(call->GetIsolate(), &userdata);

		//	Todo: Verify copy works well here
		*userdata = *value;

		call->PushObject(entity);
		return call->Return();
	}

protected:

#define CONTROLLER_NAME(name) \
	this->SetName( #name )

	const char* _name;
	void SetName(const char* name) { _name = name; }
	const char* GetName() final { return _name; }

	///	Optional destructor that can be added to enable cleaning up an object
	///	before it is free. This should remove all native references.
	///	DO NOT FREE THE OBJECT! It is freed by the scripting backend.
	virtual void OnDestroy(Entity* entity) { }

	void DestroyEntity(void* entity) final
	{
		return OnDestroy(static_cast<Entity*>(entity));
	}

protected:

	///	Add a method to this controller.
	///	This can be called at any time, but should ideally only be called at initialization.
	void AddMethod(const char* name, MethodCallback callback)
	{
		_entityCallbacks.add(_entityCallbacks.findForAdd(name), name, callback);
	}

#define CONTROLLER_METHOD(name, method) \
	this->AddMethod(#name, fastdelegate::MakeDelegate(this, method) )

	///	Add a static method to this controller.
	///	This can only be called at initialization time.
	void AddStaticMethod(const char* name, StaticMethodCallback callback)
	{
		_staticCallbacks.add(_staticCallbacks.findForAdd(name), name, callback);
	}

#define CONTROLLER_STATIC_METHOD(name, method) \
	this->AddStaticMethod(#name, fastdelegate::MakeDelegate(this, method) )

	/// Handle a script call on this entity
	IScriptResult* Namecall(IScriptCall* namecall) override
	{
		char methodName[256];
		if (!namecall->TryGetNamecall(methodName, sizeof(methodName)))
			return namecall->Error("Not a namecall!");

		auto lookup = _entityCallbacks.find(methodName);

		if (!lookup.found())
			return namecall->Error("invalid namecall");

		Entity* self = namecall->GetSelf<Entity>();
		if (self == nullptr)
			return namecall->Error("null entity");

		MethodCallback method = lookup->value;
		return method(self, namecall);
	}

	///	Handle a script call on this controller
	IScriptResult* NamecallStatic(IScriptCall* namecall) override
	{
		char methodName[256];
		if (!namecall->TryGetNamecall(methodName, sizeof(methodName)))
			return namecall->Error("Not a namecall!");

		auto lookup = _staticCallbacks.find(methodName);

		if (!lookup.found())
			return namecall->Error("invalid static namecall");

		StaticMethodCallback method = lookup->value;
		return method(namecall);
	}

private:

	ke::StringMap<MethodCallback> _entityCallbacks;
	ke::StringMap<StaticMethodCallback> _staticCallbacks;

	//std::map<const char*, MethodCallback> _entityCallbacks;
	//std::map<const char*, StaticMethodCallback> _staticCallbacks;
};

class IScriptControllerManager
{
public:
	///	Register a controller of type controller<Entity> for the global
	///	controller name "name". This controller will appear in all scripts.
	virtual IControllerInstance* Register(IBaseScriptController* controller) = 0;

	virtual void Destroy(IControllerInstance* instance) = 0;
};

#endif //DUOSCRIPT_ISCRIPTCONTROLLER_H
