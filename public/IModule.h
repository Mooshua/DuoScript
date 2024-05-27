// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under the Lesser GPL (see public/LICENSE.md), and defines an
// interface that may be linked to without distributing the linkee under a GPL license.

#ifndef DUOSCRIPT_IMODULE_H
#define DUOSCRIPT_IMODULE_H

#include <ISmmAPI.h>
#include <ISmmPlugin.h>
#include <ISmmPluginExt.h>
#include <IPluginManager.h>

#include <ILoop.h>
#include <ILogger.h>
#include <IScript.h>
#include <IScriptController.h>

class IDuoServices
{
public:
	virtual IScriptControllerManager* ScriptController() = 0;
	virtual ILoop* Loop() = 0;
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

	///	@brief Remove your plugin from DuoScript
	///	This should be called in your unload() method,
	///	while your plugin is still alive so Duo can still interact
	///	with your components.
	virtual void Goodbye(IModule* myself, PluginId* id) = 0;

	///	@brief Get a copy of the DuoScript logger
	///	You can use this to log load failures.
	virtual ILogger* GetLogger() = 0;
};


#define MODULE_NEW(classname) \
	class classname; \
	PLUGIN_GLOBALVARS()         \
    extern IDuoServices* g_Duo; \
    extern ILoop* g_DuoLoop;                          \
    extern ILogger* g_DuoLog;                          \
    extern classname g_##classname; \
	class classname : public virtual IModule

#define MODULE_BUILTINS \
	virtual void DuoLoad(IDuoServices* services); \
	virtual bool Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlength, bool late); \
    virtual bool Unload(char *error, size_t maxlength);                    \
	virtual void OnReady();\
	virtual bool QueryRunning(char* error, size_t maxlen); \
	virtual void AllPluginsLoaded();


#define MODULE_IMPL_LOAD(classname) \
    IModuleIntroducer* g_Intro_;                                \
	bool classname :: Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlength, bool late) \
	{                             \
		PLUGIN_SAVEVARS();           \
    	return true;                 \
	}                       \
	void classname :: AllPluginsLoaded() \
	{	\
		{                            \
			int ret;                         \
            int source_plugin;                        \
        	g_Intro_ =           \
				static_cast<IModuleIntroducer*>(\
				g_SMAPI->MetaFactory(INTERFACE_MODULEINTRODUCER, &ret, &source_plugin));  \
                               \
        	if (g_Intro_ == nullptr) \
				return;              \
                                    \
            g_DuoLog = g_Intro_->GetLogger();                        \
			g_Intro_->Hello(this, &g_PLID);\
		}		\
	}                                  \
	bool classname :: QueryRunning(char* error, size_t maxlen) \
	{                                  \
        if (g_Intro_ != nullptr)    \
			return true;                     \
		strcpy_s(error, maxlen, "Unable to find DuoMod!");           \
    	return false;                                \
	}

#define MODULE_IMPL_UNLOAD(classname) \
	bool classname :: Unload (char *error, size_t size) \
	{                                    \
		g_Intro_->Goodbye(this, &g_PLID);	\
		return true;								  \
	}

#define MODULE_INIT(classname) \
	PLUGIN_EXPOSE(classname, g_##classname)      \
    IDuoServices* g_Duo;       \
	ILogger* g_DuoLog;            \
    ILoop* g_DuoLoop;                           \
    classname g_##classname; 	\
                               \
	void classname :: DuoLoad(IDuoServices* services) \
	{                             \
		g_Duo = services;            \
		g_DuoLoop = services->Loop();\
	}      \
                               \
	MODULE_IMPL_LOAD(classname)   \
	MODULE_IMPL_UNLOAD(classname)

#define MODULE_LICENSE(license) \
	virtual const char *GetLicense() { return license; }

#define MODULE_NAME_SHORT(shortname) \
	virtual const char *GetLogTag() { return shortname; }


#define MODULE_AUTHOR(author, website) \
	virtual const char *GetAuthor() { return author; } \
	virtual const char *GetURL() { return website; }

#define MODULE_NAME(name, description) \
	virtual const char *GetName() { return "[DuoMod] " name; } \
	virtual const char *GetDescription() { return description; }

#define MODULE_VERSION(version) \
	virtual const char *GetVersion() { return version; } \
	virtual const char *GetDate() { return __DATE__ " at " __TIME__; }

#endif //DUOSCRIPT_IMODULE_H
