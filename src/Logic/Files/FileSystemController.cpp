// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "FileSystemController.h"
#include "Files/Files.h"

void FileRequest::Callback(uv_fs_t *req)
{
	DuoScope(FileRequest::Callback);

	FileRequest* self = static_cast<FileRequest *>(req->data);
	IScriptFiber* fiber = self->fiber->Get();

	if (!fiber)
	{
		self->_logger->Message("Filesystem", ILogger::SEV_WARN, "Fiber handle closed while yielding for filesystem open");
		delete self;

		return;
	}
	IScriptInvoke* args;

	if (!fiber->TryContinue(&args))
	{
		self->_logger->Message("Filesystem", ILogger::SEV_WARN, "Fiber handle could not be resumed from filesystem yield");
		delete self;

		return;
	}

	//	"Success" return
	args->PushBool(req->result >= 0 );

	//	Make it a tuple with the copyback, if enabled.
	if (self->copy_back && req->result >= 0)
		args->PushBuffer(self->buffer.base, self->buffer.len);
	else if (req->result < 0)
		args->PushString( uv_err_name(req->result) );

	fiber->Call(false);
	delete self;
}

void FileOpenRequest::Callback(uv_fs_t *req)
{
	DuoScope(FileOpenRequest::Callback);

	FileOpenRequest* self = static_cast<FileOpenRequest *>(req->data);
	IScriptFiber* fiber = self->fiber->Get();

	if (!fiber)
	{
		self->_logger->Message("Filesystem", ILogger::SEV_WARN, "Fiber handle closed while yielding for filesystem open");
		delete self;

		return;
	}

	IScriptInvoke* args;

	if (!fiber->TryContinue(&args))
	{
		self->_logger->Message("Filesystem", ILogger::SEV_WARN, "Fiber handle could not be resumed from filesystem yield");
		delete self;

		return;
	}

	bool success = (req->result >= 0);

	FileEntity file = FileEntity();

	if (success) {
		file.file = req->result;
		file.is_open = true;
	} else {
		file.is_open = false;
	}

	IScriptRef* entity = self->controller->VolunteerNew(args, &file);

	//	"Success" return
	args->PushBool(success);

	if (success)
		args->PushObject(entity);
	else
		args->PushString( uv_err_name(req->result) );

	fiber->Call(false);

	delete entity;
	delete self;
}

void FileStatRequest::Callback(uv_fs_t *req)
{
	DuoScope(FileStatRequest::Callback);

	FileStatRequest* self = static_cast<FileStatRequest *>(req->data);
	IScriptFiber* fiber = self->fiber->Get();

	if (!fiber)
	{
		self->_logger->Message("Filesystem", ILogger::SEV_WARN, "Fiber handle closed while yielding for filesystem open");
		delete self;

		return;
	}

	IScriptInvoke* args;

	if (!fiber->TryContinue(&args))
	{
		self->_logger->Message("Filesystem", ILogger::SEV_WARN, "Fiber handle could not be resumed from filesystem yield");
		delete self;

		return;
	}

	uv_stat_t* stat = static_cast<uv_stat_t *>(req->ptr);
	bool success = (req->result >= 0);

	//	"Success" return
	args->PushBool(success);

	if (success)
		args->PushUnsigned(stat->st_size);
	else
		args->PushString( uv_err_name(req->result) );

	fiber->Call(false);

	delete self;
}

IScriptResult *FileController::Open(IScriptCall *call)
{
	DuoScope(File::Open);

	std::string path;
	ARG_STRING(call, 1, path);

	//	Optional arg: Should we make a file if we dont have one?
	bool create = true;
	call->ArgBool(2, &create);

	FileOpenRequest* request = new FileOpenRequest(this->_logger, call->GetFiber(), this);
	{
		DuoScope(uv_fs_open);
		uv_fs_open(_loop->AsLoop(), &request->request, path.c_str(), UV_FS_O_RDWR | (create ? UV_FS_O_CREAT : 0), 0,
				   &FileOpenRequest::Callback);
	}
	return call->Await();
}

///	Read from a file
///	required length - the amount of bytes to read from the file
///	optional offset - the location to read from. default to end of last read
IScriptResult *FileController::Read(FileEntity *file, IScriptCall *call)
{
	DuoScope(File::Read);

	unsigned length;
	int offset = -1;

	ARG_UINT(call, 1, length);

	//	Optional param
	call->ArgInt(2, &offset);

	if (!file->IsValid())
		return call->Error("File object is not valid!");

	//	New copy-back request
	FileRequest* request = new FileRequest(this->_logger, call->GetFiber(), true);
	request->buffer.base = static_cast<char *>(mi_zalloc(length));
	request->buffer.len	= length;

	{
		DuoScope(uv_fs_read);
		//	Queue read request & yield for callback
		uv_fs_read(_loop->AsLoop(), &request->request, file->file, &request->buffer, 1, offset,
				   &FileRequest::Callback);
	}
	return call->Await();
}

///	Write to a file
///	required buffer - a buffer of contents to write to the file
///	optional offset - the location to write the content. default to end of file.
IScriptResult *FileController::Write(FileEntity* file, IScriptCall *call)
{
	DuoScope(File::Write);

	int offset = -1;
	std::string buffer;

	ARG_BUFFER(call, 1, buffer);

	if (!file->IsValid())
		return call->Error("File object is not valid!");

	//	optional param
	call->ArgInt(2, &offset);

	FileRequest* request = new FileRequest(this->_logger, call->GetFiber(), false);
	request->buffer.base = static_cast<char *>(mi_zalloc(buffer.size()));
	request->buffer.len = buffer.length();

	memcpy(request->buffer.base, buffer.data(), buffer.length());

	{
		DuoScope(uv_fs_write);
		uv_fs_write(_loop->AsLoop(), &request->request, file->file, &request->buffer, 1, offset,
					&FileRequest::Callback);
	}
	return call->Await();
}


IScriptResult *FileController::Length(FileEntity *file, IScriptCall *call)
{
	DuoScope(File::Length);

	FileStatRequest* request = new FileStatRequest(this->_logger, call->GetFiber());

	uv_fs_fstat(_loop->AsLoop(), &request->request, file->file, &FileStatRequest::Callback);
	return call->Await();
}


IScriptResult *FileController::GetFiles(IScriptCall *call)
{
	DuoScope(File::GetFiles);

	std::vector<std::string> files;
	std::string path = "";
	call->ArgString(1, &path);

	_files->GetFiles(&files, "%s", path.c_str());

	for (auto file : files) {
		call->PushString(file.c_str());
	}

	return call->Return();
}

IScriptResult *FileController::GetDirectories(IScriptCall *call)
{
	DuoScope(File::GetDirs);

	std::vector<std::string> files;
	std::string path = "";
	call->ArgString(1, &path);

	_files->GetDirectories(&files, "%s", path.c_str());

	for (auto file : files) {
		call->PushString(file.c_str());
	}

	return call->Return();
}


