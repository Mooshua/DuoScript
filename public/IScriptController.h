// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under the Lesser GPL (see public/LICENSE.md), and defines an
// interface that may be linked to without distributing the linkee under a GPL license.

#ifndef DUOSCRIPT_ISCRIPTCONTROLLER_H
#define DUOSCRIPT_ISCRIPTCONTROLLER_H

#include <map>

#include "Generic/StringMap.h"
#include "Generic/StringLiteral.h"
#include "IScript.h"
#include "Generic/IntMap.h"

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
	IScriptRef* Make(IIsolateHandle* vm, Entity** result = nullptr)
	{
		return this->Make(vm->Get(), (void**)result, sizeof(IScriptControllerEntity<Entity>));
	}

public:
	//	Implementation details: Do not use
	virtual IScriptRef* GetMetatable() = 0;
	virtual IScriptRef* GetStaticMetatable() = 0;
	virtual bool IsAlive() = 0;
	virtual void Kill() = 0;

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
	virtual IScriptResult* Index(IScriptCall* index) = 0;
	virtual IScriptResult* IndexStatic(IScriptCall* index) = 0;

	virtual void DestroyEntity(void* entity) = 0;

	///	An opaque pointer to our namecall, allowing
	///	the script engine to invoke our namecall without
	///	actually needing to know about us.
	fastdelegate::FastDelegate<IScriptResult*, IScriptCall*> _namecall
		= fastdelegate::MakeDelegate(this, &IBaseScriptController::Namecall);

	fastdelegate::FastDelegate<IScriptResult*, IScriptCall*> _namecallStatic
		= fastdelegate::MakeDelegate(this, &IBaseScriptController::NamecallStatic);

	fastdelegate::FastDelegate<IScriptResult*, IScriptCall*> _indexer
			= fastdelegate::MakeDelegate(this, &IBaseScriptController::Index);

	fastdelegate::FastDelegate<IScriptResult*, IScriptCall*> _indexerStatic
		= fastdelegate::MakeDelegate(this, &IBaseScriptController::IndexStatic);

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
	virtual IControllerInstance* GetInstance() = 0;
	virtual IBaseScriptController* GetController() = 0;
	virtual void* GetEntity() = 0;

	///	@brief Attempt to convert an entity to the desired type
	///	If this is successful, it will return true and write the pointer
	///	to the entity in *result.
	///
	///	The pointer is NOT yield safe; do NOT toss this pointer across
	///	returns or yield calls: this is UNDEFINED BEHAVIOR.
	template<typename EntityType>
	bool TryGetEntity(EntityType** result)
	{
		EntityType* entity = dynamic_cast<EntityType*>(this->GetEntity());

		if (entity == nullptr)
			return false;

		*result = entity;
	}
};

//	Dummy entity for people who don't want an entity
//	(static controllers)
class NoEntity
{

};

template<typename Entity>
class IScriptControllerEntity : IBaseScriptControllerEntity
{
public:
	IScriptControllerEntity(IControllerInstance* instance, IBaseScriptController* controller)
		: _instance(instance)
		, _controller(controller)
	{}
	void Initialize(IControllerInstance* instance, IBaseScriptController* controller)
	{
		this->_instance = instance;
		this->_controller = controller;
	}

	IControllerInstance* GetInstance() final { return _instance; }
	IBaseScriptController* GetController() final { return _controller; }
	void* GetEntity() final { return &_entity; }

	//	Allow anyone who knows the template type
	//	to avoid static_cast-ing to get it.
	inline Entity* AsEntity() { return &_entity; }

private:
	IControllerInstance* _instance;
	IBaseScriptController* _controller;
	Entity _entity;
};

///	A script controller handles requests for
///	entities in the script world. Entities hold the state,
///	while controllers process requests for the entity.
template<typename Entity = NoEntity>
class IScriptController : public virtual IBaseScriptController
{

	friend class IScript;
	friend class IScriptControllerManager;
public:
	typedef fastdelegate::FastDelegate<IScriptResult*, Entity*, IScriptCall*> MethodCallback;
	typedef fastdelegate::FastDelegate<IScriptResult*, IScriptCall*> StaticMethodCallback;
	typedef fastdelegate::FastDelegate<IScriptResult*, Entity*, IScriptCall*> Getter;
	typedef fastdelegate::FastDelegate<IScriptResult*, Entity*, IScriptCall*> Setter;


	///	The type that will be instantiated in the script VM
	typedef IScriptControllerEntity<Entity> Userdata;
	typedef Entity ControllerEntity;

	IScriptController()
	{
		_entityCallbacks.init();
		_fastEntityCallbacks.init();

		_staticCallbacks.init();

		_entityGet.init();
	}

protected:

	///	Return to an IScriptCall with a new entity
	IScriptResult* ReturnNew(IScriptCall* call, Entity* value)
	{
		Userdata* userdata;
		IScriptRef* entity = _template->Make(call->GetIsolate(), &userdata);

		//	Welcome to another episode of "C++ sucks ass"
		//	Because of stupid "smart" objects trying to
		//	get their greasy, knobby little hands into uninitialized memory,
		//	we have to do THIS bullshit. It's called a "placement constructor".
		userdata->Initialize(_template, this);
		new (userdata->AsEntity()) Entity(*value);

		call->PushObject(entity);
		return call->Return();
	}

	///	Volunteer an entity as a script reference.
	///	This is used when we're part of an interface, but not
	///	directly handling the call's response.
	IScriptRef* VolunteerNew(IScriptInvoke* call, Entity* value)
	{
		Userdata* userdata;
		IScriptRef* entity = _template->Make(call->GetIsolate(), &userdata);

		userdata->Initialize(_template, this);
		new (userdata->AsEntity()) Entity(*value);

		return entity;
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
		auto insert = _entityCallbacks.findForAdd(name);
		_entityCallbacks.add(insert, name, callback);
	}

#define CONTROLLER_METHOD(name, method) \
	this->AddMethod(#name, fastdelegate::MakeDelegate(this, method) )

	///	Add a static method to this controller.
	///	This can only be called at initialization time.
	void AddStaticMethod(const char* name, StaticMethodCallback callback)
	{
		auto insert = _staticCallbacks.findForAdd(name);
		_staticCallbacks.add(insert, name, callback);
	}

#define CONTROLLER_STATIC_METHOD(name, method) \
	this->AddStaticMethod(#name, fastdelegate::MakeDelegate(this, method) )

	void AddGetter(const char* name, Getter callback)
	{
		auto insert = _entityGet.findForAdd(name);
		_entityGet.add(insert, name, callback);
	}

#define CONTROLLER_GETTER(name, method) \
	this->AddGetter(#name, fastdelegate::MakeDelegate(this, method) )

#define CONTROLLER_AUTO_GETTER(name, type, memberptr) \
	this->AddGetter(#name, &controller_getter<type, ControllerEntity, memberptr>);

	IScriptResult* Index(IScriptCall* index)
	{
		char key[256];
		if (!index->ArgString(1, key, sizeof(key)))
			return index->Error("Cannot find key %s", key);

		auto lookup = _entityGet.find(key);
		if (!lookup.found())
			return index->Error("Could not find key '%s' in type %s", key, _name);

		Entity* self = index->GetSelf<Userdata>()->AsEntity();
		if (self == nullptr)
			return index->Error("Invalid __index invocation (null entity)");

		Getter get = lookup->value;
		return get(self, index);
	}

	IScriptResult* IndexStatic(IScriptCall* index)
	{
		return index->Error("Static indexer not supported on controller '%s'", this->GetName());
	}

	/// Handle a script call on this entity
	IScriptResult* Namecall(IScriptCall* namecall) override
	{
		DuoScope(ScriptController::Dispatch);

		char methodName[256];
		int atom;

		if (!namecall->TryGetNamecall(methodName, &atom, sizeof(methodName)))
			return namecall->Error("Not a namecall!");

		Entity* self = namecall->GetSelf<Userdata>()->AsEntity();
		if (self == nullptr)
			return namecall->Error("null entity");

		//	Fast-path: Do we have a known atom for this namecall?
		if (atom != ATOM_UNDEF)
		{
			auto atomLookup = _fastEntityCallbacks.find(atom);

			if (atomLookup.found())
				return atomLookup->value(self, namecall);
		}

		DuoScope(ScriptController::Dispatch - Slow Path);
		auto lookup = _entityCallbacks.find(methodName);

		if (!lookup.found())
			return namecall->Error("invalid namecall %s", methodName);

		//	We have a string for this but no atom yet.
		//	Let's patch the atom in now:
		auto insert = _fastEntityCallbacks.findForAdd(atom);
		_fastEntityCallbacks.add(insert, atom, lookup->value);

		return lookup->value(self, namecall);
	}

	///	Handle a script call on this controller
	IScriptResult* NamecallStatic(IScriptCall* namecall) override
	{
		DuoScope(ScriptController::Dispatch);

		char methodName[256];
		if (!namecall->TryGetNamecall(methodName, nullptr, sizeof(methodName)))
			return namecall->Error("Not a namecall!");

		auto lookup = _staticCallbacks.find(methodName);

		if (!lookup.found())
			return namecall->Error("invalid static namecall %s", methodName);

		StaticMethodCallback method = lookup->value;
		return method(namecall);
	}

private:

	//ke::StringMap<MethodCallback> _entityCallbacks;
	ke::StringMap<MethodCallback> _entityCallbacks;
	ke::IntMap<MethodCallback> _fastEntityCallbacks;

	ke::StringMap<StaticMethodCallback> _staticCallbacks;

	ke::StringMap<Getter> _entityGet;
	ke::StringMap<Setter> _entitySet;

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


template<typename Type, typename Entity, Type Entity::* Pointer>
IScriptResult* controller_getter(Entity* entity, IScriptCall* call)
{
	script_push(call, entity->*Pointer);
	return call->Return();
}

#endif //DUOSCRIPT_ISCRIPTCONTROLLER_H
