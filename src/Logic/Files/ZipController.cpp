// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "ZipController.h"

void ZipOpenTask::Execute()
{
	int error;
	zip_t* file = zip_openwitherror(this->_path.c_str(), ZIP_DEFAULT_COMPRESSION_LEVEL, this->_mode, &error);

	if (file == nullptr || error != 0)
	{
		//	An error occured!
		this->_result = nullptr;
		this->_error = zip_strerror(error);
		return;
	}

	this->_result = file;
}

void ZipOpenTask::Finish()
{
	if (!this->_fiber->Exists())
		return;

	IScriptFiber* fiber = this->_fiber->Get();

	if (this->_result == nullptr)
	{
		//	Error!
		fiber->Kill("Error opening %s: %s", this->_path.c_str(), this->_error.c_str());
		return;
	}

	ZipEntity entity = ZipEntity(this->_result);

	IScriptInvoke* args;
	if (!fiber->TryContinue(&args))
		return;
	{
		args->PushObject( _parent->VolunteerNew(args, &entity));
	}
	fiber->Call(false);
}

IScriptResult *ZipController::Open(IScriptCall *call)
{
	std::string path;
	if (!call->ArgString(1, &path))
		return call->Error("Expected argument 1 to be a string!");

	bool read;
	if (!call->ArgBool(2, &read))
		return call->Error("Expected argument 2 to be a boolean! (read)");

	_loop->NewTask(new ZipOpenTask(this, call->GetFiber(), path, read ? 'r' : 'w'));
	return call->Await();
}

IScriptResult *ZipController::Read(ZipEntity *entity, IScriptCall *call)
{
	if (entity->_file == nullptr)
		return call->Error("zip file has already been closed!");

	std::string path;
	if (!call->ArgString(1, &path))
		return call->Error("Expected argument 1 to be a string!");

	int status = zip_entry_open(entity->_file, path.c_str());
	if (status != 0)
		return call->Error("Error reading zip entry %s: %s", path.c_str(), zip_strerror(status));

	size_t size = zip_entry_size(entity->_file);
	std::string contents = std::string(size + 1, '\0');

	//	This only fails when this->_file is uninitialized/closed
	//	or when we're out of memory. So we don't care about the return :)
	zip_entry_noallocread(entity->_file, contents.data(), size);
	zip_entry_close(entity->_file);

	//	Push the file contents into the lua VM as a buffer
	call->PushBuffer(contents.data(), size);
	return call->Return();
}

IScriptResult *ZipController::Write(ZipEntity *entity, IScriptCall *call)
{
	if (entity->_file == nullptr)
		return call->Error("zip file has already been closed!");

	std::string path;
	if (!call->ArgString(1, &path))
		return call->Error("Expected argument 1 to be a string!");

	std::string contents;
	if (!call->ArgBuffer(2, &contents))
		return call->Error("Expected argument 2 to be a buffer!");

	{
		//	if the entry already exists, delete it
		char* const entries_to_delete[] = { path.data() };
		zip_entries_delete(entity->_file, entries_to_delete, 1);
	}

	int status = zip_entry_open(entity->_file, path.c_str());
	if (status != 0)
		return call->Error("Error writing zip entry %s: %s", path.c_str(), zip_strerror(status));

	zip_entry_write(entity->_file, contents.data(), contents.size());
	zip_entry_close(entity->_file);

	return call->Return();
}

IScriptResult *ZipController::Append(ZipEntity *entity, IScriptCall *call)
{
	if (entity->_file == nullptr)
		return call->Error("zip file has already been closed!");

	std::string path;
	if (!call->ArgString(1, &path))
		return call->Error("Expected argument 1 to be a string!");

	std::string contents;
	if (!call->ArgBuffer(2, &contents))
		return call->Error("Expected argument 2 to be a buffer!");

	int status = zip_entry_open(entity->_file, path.c_str());
	if (status != 0)
		return call->Error("Error appending zip entry %s: %s", path.c_str(), zip_strerror(status));

	zip_entry_write(entity->_file, contents.data(), contents.size());
	zip_entry_close(entity->_file);

	return call->Return();
}

IScriptResult *ZipController::Close(ZipEntity *entity, IScriptCall *call)
{
	if (entity->_file == nullptr)
		return call->Error("zip file has already been closed!");

	zip_close(entity->_file);
	entity->_file = nullptr;

	return call->Return();
}
