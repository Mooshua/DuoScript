// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include <mimalloc.h>
#include "Files.h"
#include "Helpers/Path.h"
#include <IProfiler.h>

#define GET_PATH(path) \
	va_list args; \
	va_start(args, fmt); \
	duo::BuildPathEx(path, sizeof(path), fmt, args); \
	va_end(args);

bool Files::TryGetChildren(std::vector<std::string> *children, const char *path, uv_dirent_type_t type)
{
	uv_dir_t* directory;
	uv_fs_t request;
	uv_dirent_t dirent;

	if (!this->TryOpenDirectory(path, &directory))
		return false;

	uv_fs_scandir(nullptr, &request, path, 0, nullptr);

	while (uv_fs_scandir_next(&request, &dirent) == 0)
	{
		if (dirent.type == type)
			children->emplace_back( dirent.name );
	}

	return true;
}

bool Files::TryGetStat(const char *path, uv_stat_t* stat)
{
	uv_fs_t request;

	//	No need to use loop/cb as we're running synchronously
	int status = uv_fs_stat(nullptr, &request, path, nullptr);

	if (status != 0 || request.result != 0) {
		uv_fs_req_cleanup(&request);
		return false;
	}

	//	req->ptr is a uv_statfs_t object now.
	//	copy it to result buffer
	*stat = request.statbuf;
	uv_fs_req_cleanup(&request);

	return true;
}

bool Files::TryOpenDirectory(const char *path, uv_dir_t **directory)
{
	uv_fs_t request;
	int status = uv_fs_opendir(nullptr, &request, path, nullptr);

	if (status != 0)
	{
		//	No cleanup required here?
		return false;
	}

	*directory = static_cast<uv_dir_t *>(request.ptr);
	return true;
}

void Files::GetFiles(std::vector<std::string> *files, const char *fmt, ...)
{
	DuoScope(Files::GetFiles - Synchronous);

	char path[256];
	GET_PATH(path);

	this->TryGetChildren(files, path, UV_DIRENT_FILE);
}

void Files::GetDirectories(std::vector<std::string> *directories, const char *fmt, ...)
{
	DuoScope(Files::GetDirectories - Synchronous);

	char path[256];
	GET_PATH(path);

	this->TryGetChildren(directories, path, UV_DIRENT_DIR);
}

bool Files::IsFile(const char *fmt, ...)
{
	DuoScope(Files::IsFile - Synchronous);

	char path[256];
	GET_PATH(path);

	uv_stat_t stat;
	if (!this->TryGetStat(path, &stat))
		return false;

	return (stat.st_mode & UV_FS_O_DIRECTORY) == 0;
}

bool Files::IsFolder(const char *fmt, ...)
{
	DuoScope(Files::IsFolder - Synchronous);

	char path[256];
	GET_PATH(path);

	uv_stat_t stat;
	if (!this->TryGetStat(path, &stat))
		return false;

	return (stat.st_mode & UV_FS_O_DIRECTORY) == UV_FS_O_DIRECTORY;}
