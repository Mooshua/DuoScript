// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "FileSystemController.h"
#include "LogicGlobals.h"
#include "Files/Files.h"

FileController g_FileController;

void FileRequest::Callback(uv_fs_t *req)
{
	FileRequest* self = static_cast<FileRequest *>(req->data);

	if (!self->fiber->Exists())
	{
		delete self;
		return g_DuoLog->Message("Filesystem", ILogger::SEV_WARN, "Fiber handle closed while yielding for filesystem");
	}

	IScriptFiber* fiber = self->fiber->Get();
	IScriptInvoke* args;

	if (!fiber->TryContinue(&args))
	{
		delete self;
		return g_DuoLog->Message("Filesystem", ILogger::SEV_WARN, "Fiber handle could not be resumed from filesystem yield");
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
	FileOpenRequest* self = static_cast<FileOpenRequest *>(req->data);

	if (!self->fiber->Exists())
	{
		delete self;
		return g_DuoLog->Message("Filesystem", ILogger::SEV_WARN, "Fiber handle closed while yielding for filesystem open");
	}

	IScriptFiber* fiber = self->fiber->Get();
	IScriptInvoke* args;

	if (!fiber->TryContinue(&args))
	{
		delete self;
		return g_DuoLog->Message("Filesystem", ILogger::SEV_WARN, "Fiber handle could not be resumed from filesystem open yield");
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

IScriptResult *FileController::Open(IScriptCall *call)
{
	char path[256];
	if (!call->ArgString(1, path, sizeof(path)))
		call->Error("Expected argument 1 to be a string!");

	//	Optional arg: Should we make a file if we dont have one?
	bool create = true;
	call->ArgBool(2, &create);

	FileOpenRequest* request = new FileOpenRequest(call->GetFiber(), this);
	uv_fs_open(g_DuoLoop->AsLoop(), &request->request, path, UV_FS_O_RDWR | (create ? UV_FS_O_CREAT : 0), 0, &FileOpenRequest::Callback);

	return call->Await();
}

///	Read from a file
///	required length - the amount of bytes to read from the file
///	optional offset - the location to read from. default to end of last read
IScriptResult *FileController::Read(FileEntity *file, IScriptCall *call)
{
	unsigned length;
	int offset = -1;

	if (!call->ArgUnsigned(1, &length))
		return call->Error("Expected argument 1 to be an integer!");

	//	Optional param
	call->ArgInt(2, &offset);

	if (!file->IsValid())
		return call->Error("File object is not valid!");

	//	New copy-back request
	FileRequest* request = new FileRequest(call->GetFiber(), true);
	request->buffer.base = static_cast<char *>(mi_zalloc(length));
	request->buffer.len	= length;

	//	Queue read request & yield for callback
	uv_fs_read(g_DuoLoop->AsLoop(), &request->request, file->file, &request->buffer, 1, offset, &FileRequest::Callback);
	return call->Await();
}

///	Write to a file
///	required buffer - a buffer of contents to write to the file
///	optional offset - the location to write the content. default to end of file.
IScriptResult *FileController::Write(FileEntity* file, IScriptCall *call)
{
	int offset = -1;
	char* buffer;
	size_t length;

	if (!call->ArgBuffer(1, reinterpret_cast<void **>(&buffer), &length))
		return call->Error("Expected argument 1 to be a buffer! (use buffer.fromstring)");

	if (!file->IsValid())
		return call->Error("File object is not valid!");

	//	optional param
	call->ArgInt(2, &offset);

	FileRequest* request = new FileRequest(call->GetFiber(), false);
	request->buffer.base = static_cast<char *>(mi_zalloc(length));
	request->buffer.len = length;

	memcpy(request->buffer.base, buffer, length);

	uv_fs_write(g_DuoLoop->AsLoop(), &request->request, file->file, &request->buffer, 1, offset, &FileRequest::Callback);
	return call->Await();
}

IScriptResult *FileController::GetFiles(IScriptCall *call)
{
	std::vector<std::string> files;
	std::string path = "";
	call->ArgString(1, &path);

	g_Files.GetFiles(&files, "%s", path.c_str());

	for (auto file : files) {
		call->PushString(file.c_str());
	}

	return call->Return();
}

IScriptResult *FileController::GetDirectories(IScriptCall *call)
{
	std::vector<std::string> files;
	std::string path = "";
	call->ArgString(1, &path);

	g_Files.GetDirectories(&files, "%s", path.c_str());

	for (auto file : files) {
		call->PushString(file.c_str());
	}

	return call->Return();
}