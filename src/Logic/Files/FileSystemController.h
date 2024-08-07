// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#ifndef DUOSCRIPT_FILESYSTEMCONTROLLER_H
#define DUOSCRIPT_FILESYSTEMCONTROLLER_H

#include "uv.h"

#include <IScriptController.h>
#include <IScript.h>
#include <IFiles.h>

#include "mimalloc.h"
#include "ILogger.h"

class FileController;
class FileEntity;

///	Object wrapping uv_fs requests
///	Optional buffer for storing/readback that is freed on callback
class FileRequest
{
public:
	FileRequest(ILogger* logger, IFiberHandle* fiber, bool copy_back)
	{
		this->_logger = logger;
		this->fiber = fiber;
		this->request.data = this;
		this->buffer.base = nullptr;
		this->copy_back = copy_back;
	}
	~FileRequest()
	{
		uv_fs_req_cleanup(&this->request);

		delete this->fiber;
		if (this->buffer.base != nullptr)
			mi_free(this->buffer.base);
	}

	static void Callback(uv_fs_t *req);
public:
	ILogger* _logger;

	///	The libuv request type
	uv_fs_t request;

	uv_buf_t buffer;

	///	If true, we should copy back the buffer
	///	into the script engine once we are done.
	bool copy_back;

	///	The fiber to be resumed
	IFiberHandle* fiber;
};

class FileStatRequest
{
public:
	FileStatRequest(ILogger* logger, IFiberHandle* fiber)
	{
		this->_logger = logger;
		this->request.data = this;
		this->fiber = fiber;
	}
	~FileStatRequest()
	{
		uv_fs_req_cleanup(&this->request);

		delete this->fiber;
	}
	static void Callback(uv_fs_t *req);
public:
	ILogger* _logger;

	uv_fs_t request;

	IFiberHandle* fiber;
};

class FileOpenRequest
{
public:
	FileOpenRequest(ILogger* logger, IFiberHandle* callback, FileController* controller)
	{
		this->_logger = logger;
		this->request.data = this;
		this->fiber = callback;
		this->controller = controller;
	}
	~FileOpenRequest()
	{
		uv_fs_req_cleanup(&this->request);

		delete this->fiber;
	}

	static void Callback(uv_fs_t *req);
public:
	ILogger* _logger;

	uv_fs_t request;

	IFiberHandle* fiber;
	FileController* controller;
};

class FileEntity
{
public:
	bool IsValid()
	{
		return is_open;
	}
public:
	///	True when the handle is still open.
	bool is_open;

	///	Underlying file handle
	uv_file file;
};

class FileController : public virtual IScriptController<FileEntity>
{
	friend class FileOpenRequest;

	ILoop* _loop;
	ILogger* _logger;
	IFiles* _files;
public:
	FileController(ILogger* logger, ILoop* loop, IFiles* files)
	{
		this->_logger = logger;
		this->_loop = loop;
		this->_files = files;

		CONTROLLER_NAME(File);
		CONTROLLER_STATIC_METHOD(Open, &FileController::Open);

		CONTROLLER_STATIC_METHOD(GetFiles, &FileController::GetFiles);
		CONTROLLER_STATIC_METHOD(GetDirs, &FileController::GetDirectories);

		CONTROLLER_METHOD(Read, &FileController::Read);
		CONTROLLER_METHOD(Write, &FileController::Write);
		CONTROLLER_METHOD(Length, &FileController::Length);
	}

	IScriptResult* Open(IScriptCall* call);

	IScriptResult* GetFiles(IScriptCall* call);
	IScriptResult* GetDirectories(IScriptCall* call);

	IScriptResult* Read(FileEntity* file, IScriptCall* call);
	IScriptResult* Write(FileEntity* file, IScriptCall* call);
	IScriptResult* Length(FileEntity* file, IScriptCall* call);

};

#endif //DUOSCRIPT_FILESYSTEMCONTROLLER_H
