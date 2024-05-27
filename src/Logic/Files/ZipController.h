// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_ZIPCONTROLLER_H
#define DUOSCRIPT_ZIPCONTROLLER_H

#include <zip.h>
#include <IScriptController.h>

class ZipController;

class ZipEntity
{
public:
	ZipEntity() = default;
	ZipEntity(zip_t* file)
	{
		this->_file = file;
	}
public:
	zip_t* _file;
};

class ZipOpenTask : public ITask
{
public:
	ZipOpenTask(ZipController* parent, IFiberHandle* handle, std::string path, char mode)
	{
		this->_parent = parent;

		this->_fiber = handle;
		this->_path = path;
		this->_mode = mode;

		this->_result = nullptr;
	}
	~ZipOpenTask()
	{
		delete this->_fiber;
	}
protected:
	virtual void Execute();
	virtual void Finish();
protected:
	ZipController* _parent;

	IFiberHandle* _fiber;
	std::string _path;
	char _mode;

	std::string _error;
	zip_t* _result;
};

class ZipController : public IScriptController<ZipEntity>
{
	friend class ZipOpenTask;
public:
	ZipController()
	{
		CONTROLLER_NAME(Zip);

		CONTROLLER_STATIC_METHOD(Open, &ZipController::Open);
		CONTROLLER_METHOD(Read, &ZipController::Read);
		CONTROLLER_METHOD(Write, &ZipController::Write);
		CONTROLLER_METHOD(Append, &ZipController::Append);

		CONTROLLER_METHOD(Close, &ZipController::Close);

	}

protected:
	void OnDestroy(ZipEntity* entity) override
	{
		//	Close the zip handle if it was not already closed
		if (entity->_file != nullptr)
			zip_close(entity->_file);

		entity->_file = nullptr;
	}
public:
	IScriptResult* Open(IScriptCall* call);

	IScriptResult* Read(ZipEntity* entity, IScriptCall* call);
	IScriptResult* Write(ZipEntity* entity, IScriptCall* call);
	IScriptResult* Append(ZipEntity* entity, IScriptCall* call);

	IScriptResult* Close(ZipEntity* entity, IScriptCall* call);
};

extern ZipController g_ZipController;


#endif //DUOSCRIPT_ZIPCONTROLLER_H
