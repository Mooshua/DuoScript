// Copyright (c) 2024 Mooshua. All rights reserved.

#include "ScriptControllers.h"
#include "ScriptVM.h"
#include "ScriptObject.h"

ControllerInstance::ControllerInstance(IScriptRef* metatable, IScriptRef* staticMetatable)
{
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
	return new ScriptRef(isolate->L, ref);
}

IControllerInstance* ScriptControllerManager::Register(IBaseScriptController* controller)
{
	IScriptRef* namecall = g_ScriptVM.NewMethod(&controller->_namecall);
	IScriptRef* staticNamecall = g_ScriptVM.NewMethod(&controller->_namecallStatic);

	//	=====================================
	//	Create new userdata for global object
	lua_newuserdata(L, 0);
		//	Create global metatable
		lua_newtable(L);
			lua_getref(L, staticNamecall->AsReferenceId());
				lua_rawsetfield(L, -2, "__namecall");

			lua_setreadonly(L, -1, true);
		lua_setmetatable(L, -2);
	int staticMetatable = lua_ref(L, -1);

	//	little cha-cha slide here
	lua_getref(L, staticMetatable);
	lua_setglobal(L, controller->GetName());

	//	=====================================
	//	Create entity metatable
	lua_newtable(L);

		lua_getref(L, namecall->AsReferenceId());
			lua_rawsetfield(L, -2, "__namecall");

		lua_setreadonly(L, -1, true);
	int metatable = lua_ref(L, -1);

	//	Delete references to methods
	//	(They're now in our tables)
	delete namecall;
	delete staticNamecall;

	IControllerInstance* entityTemplate = new ControllerInstance(
			new ScriptRef(L, metatable),
			new ScriptRef(L, staticMetatable));
	controller->SetTemplate(entityTemplate);

	return entityTemplate;
}

void ScriptControllerManager::OnDestroy(void *userdata)
{
	IBaseScriptControllerEntity* entity = static_cast<IBaseScriptControllerEntity*>(userdata);
	IBaseScriptController* controller = entity->GetController();

	controller->DestroyEntity(entity->GetEntity());
}

void ScriptControllerManager::Destroy(IControllerInstance *instance)
{
	//	We should clear all tables referencing our controller
	//	metatables. This allows the __namecall method to be garbage
	//	collected, IF none of the plugins try and do anything shady by holding onto it.

	//	This will cause a crash when any plugin attempts to access an entity
	//	or this controller. This is intentional, as it allows the controller
	//	to be safely removed from memory.

	lua_getref(L, instance->GetMetatable()->AsReferenceId());
	lua_setreadonly(L, -1, false);
	lua_cleartable(L, -1);
	lua_setreadonly(L, -1, true);
	lua_pop(L, 1);

	lua_getref(L, instance->GetStaticMetatable()->AsReferenceId());
	lua_cleartable(L, -1);
	lua_setreadonly(L, -1, false);
	lua_setreadonly(L, -1, true);
	lua_pop(L, 1);

}
