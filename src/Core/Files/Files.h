// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_FILES_H
#define DUOSCRIPT_FILES_H

#include <IFiles.h>
#include <uv.h>

class Files
{
public:
	//virtual bool IsFile(const char* fmt, ...);
	//virtual bool IsFolder(const char* fmt, ...);

	virtual void GetFiles(std::vector<std::string> *files, const char* fmt, ...);
	virtual void GetDirectories(std::vector<std::string> *directories, const char* fmt, ...);

public:
	//virtual void Read(std::string *contents, const char* fmt, ...);
	//virtual void Write(std::string *newContents, const char* fmt, ...);

protected:
	bool TryGetChildren(std::vector<std::string> *children, const char* path, uv_dirent_type_t type);
	bool TryOpenDirectory(const char* path, uv_dir_t** directory);
	bool TryGetStat(const char* path, uv_statfs_t* stat);
};

extern Files g_Files;

#endif //DUOSCRIPT_FILES_H
