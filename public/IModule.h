// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_IMODULE_H
#define DUOSCRIPT_IMODULE_H

#include <metamod.h>

#include <ILoop.h>
#include <IScript.h>
#include <IScriptController.h>

class IDuoServices
{
public:
	virtual IScriptControllerManager* ScriptController() = 0;
};

class IModule : public virtual ISmmPlugin
{
public:
	virtual void DuoLoad(IDuoServices* services) = 0;

	virtual void OnReady() = 0;
};


#define INTERFACE_MODULEINTRODUCER "IModuleIntroducer001"
#define INTERFACE_MODULEINTRODUCER_001 "IModuleIntroducer001"

class IModuleIntroducer
{
public:
	///	@brief Introduce your module to DuoScript.
	///	This should be called exactly once.
	///
	///	If your module is not loaded through DuoScript
	///	and this is called, this has undefined behavior.
	///	(please only use this as a DuoScript module!)
	///
	///	This will call myself->DuoLoad() sometime in the future
	///	with a pointer to your plugin services sink.
	///	as long as you are loaded through duo, it is perfectly
	///	safe to never dispose of resources acquired through the
	///	sink, **EXCEPT** for scripting handles/refs, as those will
	///	cause memory leaks if they aren't freed.
	///
	///	@param myself Your ISmmPlugin instance
	virtual void Hello(IModule* myself, PluginId* id) = 0;
};


#define MODULE_NEW(classname) \
	class classname; \
	PLUGIN_GLOBALVARS()         \
    extern IDuoServices* g_Duo;\
    extern classname g_##classname; \
	class classname : public virtual IModule

#define MODULE_BUILTINS \
	virtual void DuoLoad(IDuoServices* services); \
	virtual bool Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlength, bool late); \

#define MODULE_IMPL_LOAD \
	bool classname :: Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlength, bool late) \
	{                             \
		PLUGIN_SAVEVARS();           \
                               \
		{                            \
			int ret;            \
        	IModuleIntroducer* intro =           \
				static_cast<IModuleIntroducer*>(\
				ismm->MetaFactory(INTERFACE_MODULEINTRODUCER, &ret, &id));  \
                               \
        	if (intro == nullptr) \
				return false;              \
                               \
			intro->Hello(this, &id);\
		}		\
                               \
                               \
    	return true;                 \
	}

#define MODULE_INIT(classname) \
	PLUGIN_EXPOSE(classname, g_##classname)      \
    IDuoServices* g_Duo;       \
    classname g_##classname; \
                               \
	void classname :: DuoLoad(IDuoServices* services) \
	{ g_Duo = services; }      \
                               \
	MODULE_IMPL_LOAD 			\




#define MODULE_AUTHOR(author) \
	virtual const char *GetAuthor() { return #author; }

#define MODULE_NAME(name) \
	virtual const char *GetName() { return #name; }

#endif //DUOSCRIPT_IMODULE_H
