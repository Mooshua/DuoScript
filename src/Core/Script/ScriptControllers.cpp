// Copyright (c) 2024 Mooshua. All rights reserved.

#include "ScriptControllers.h"
#include "ScriptVM.h"
#include "ScriptObject.h"
#include "Logging/Log.h"

ControllerInstance::ControllerInstance(IScriptRef* metatable, IScriptRef* staticMetatable)
{
	this->_killed = false;

	this->metatable = metatable;
	this->staticMetatable = staticMetatable;
}

ControllerInstance::~ControllerInstance()
{
	delete this->metatable;
	delete this->staticMetatable;
}

IScriptRef* ControllerInstance::Make(IScriptIsolate* vm, void** result, int size)
{
	ScriptIsolate* isolate = static_cast<ScriptIsolate*>(vm);

	void *userdata = lua_newuserdatadtor(isolate->L, size, &ScriptControllerManager::OnDestroy);

	if (result != nullptr)
		*result = userdata;

	//	push metatable, pop metatable and apply to udata
	lua_getref(isolate->L, metatable->AsReferenceId());
	lua_setmetatable(isolate->L, -2);

	int ref = lua_ref(isolate->L, -1);
	lua_pop(isolate->L, 1);

	return new ScriptRef(isolate->L, ref);
}

IControllerInstance* ScriptControllerManager::Register(IBaseScriptController* controller)
{
	IScriptRef* namecall = g_ScriptVM.NewMethod(&controller->_namecall);
	IScriptRef* staticNamecall = g_ScriptVM.NewMethod(&controller->_namecallStatic);
	IScriptRef* index = g_ScriptVM.NewMethod(&controller->_indexer);

	//	=====================================
	//	Create new userdata for global object
	lua_newuserdata(L, 0);
		//	Create global metatable
		lua_newtable(L);

			lua_getref(L, staticNamecall->AsReferenceId());
				lua_rawsetfield(L, -2, "__namecall");

			//	Don't allow scripts to read the metatable,
			//	as they can hold on to method refs after their modules
			//	have been unloaded (crashy, crashy! and unsafe.)
			lua_newtable(L);
				lua_setreadonly(L, -1, true);
				lua_rawsetfield(L, -2, "__metatable");

			lua_setreadonly(L, -1, true);
			int staticMetatable = lua_ref(L, -1);
		lua_setmetatable(L, -2);
	lua_pop(L, 1);

	//	little cha-cha slide here
	lua_getref(L, staticMetatable);
	lua_setglobal(L, controller->GetName());

	//	=====================================
	//	Create entity metatable
	lua_newtable(L);

		lua_getref(L, namecall->AsReferenceId());
			lua_rawsetfield(L, -2, "__namecall");

		lua_getref(L, index->AsReferenceId());
			lua_rawsetfield(L, -2, "__index");

		//	Don't allow scripts to read the metatable,
		//	as they can hold on to method refs after their modules
		//	have been unloaded (crashy, crashy! and unsafe.)
		lua_newtable(L);
			lua_setreadonly(L, -1, true);
			lua_rawsetfield(L, -2, "__metatable");

		lua_setreadonly(L, -1, true);
		int metatable = lua_ref(L, -1);
	lua_pop(L, 1);

	//	Delete references to methods
	//	(They're now in our tables)
	delete namecall;
	delete index;
	delete staticNamecall;

	IControllerInstance* entityTemplate = new ControllerInstance(
			new ScriptRef(L, metatable),
			new ScriptRef(L, staticMetatable));
	controller->SetTemplate(entityTemplate);

	g_Log.Message("ScriptController", Log::SEV_DEBUG, "Registered controller %s", controller->GetName());

	return entityTemplate;
}

void ScriptControllerManager::OnDestroy(void *userdata)
{
	IBaseScriptControllerEntity* entity = static_cast<IBaseScriptControllerEntity*>(userdata);

	//	The controller instance can be killed, but this
	//	is not propagated to the entity. The controller field
	//	is undefined when this happens, so check before we deref.
	if (!entity->GetInstance()->IsAlive())
		return;

	IBaseScriptController* controller = entity->GetController();
	controller->DestroyEntity(entity->GetEntity());
}

void ScriptControllerManager::Destroy(IControllerInstance *instance)
{
	//	We should clear all tables referencing our controller
	//	metatables. This allows the __namecall method to be garbage
	//	collected, IF none of the plugins try and do anything shady by holding onto it.

	//	This will cause am error when any plugin attempts to access an entity
	//	or this controller. This is intentional, as it allows the controller
	//	to be safely removed from memory.

	lua_getref(L, instance->GetMetatable()->AsReferenceId());
	lua_setreadonly(L, -1, false);
	lua_cleartable(L, -1);
	lua_setreadonly(L, -1, true);
	lua_pop(L, 1);

	lua_getref(L, instance->GetStaticMetatable()->AsReferenceId());
	lua_setreadonly(L, -1, false);
	lua_cleartable(L, -1);
	lua_setreadonly(L, -1, true);
	lua_pop(L, 1);

	//	mark the instance as "killed" to prevent people from accessing the controller
	//	this protects all entities from the controller being unallocated.
	//	(specifically, this is for dtors!)
	instance->Kill();

	g_Log.Message("ScriptController", Log::SEV_DEBUG, "Removed a controller");
}
