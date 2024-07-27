// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#ifndef DUOSCRIPT_FILES_H
#define DUOSCRIPT_FILES_H

#include <uv.h>
#include <IFiles.h>

class Files : public IFiles
{
public:
	virtual bool IsFile(const char* fmt, ...);
	virtual bool IsFolder(const char* fmt, ...);

	virtual void GetFiles(std::vector<std::string> *files, const char* fmt, ...);
	virtual void GetDirectories(std::vector<std::string> *directories, const char* fmt, ...);

public:
	//virtual void Read(std::string *contents, const char* fmt, ...);
	//virtual void Write(std::string *newContents, const char* fmt, ...);

protected:
	bool TryGetChildren(std::vector<std::string> *children, const char* path, uv_dirent_type_t type);
	bool TryOpenDirectory(const char* path, uv_dir_t** directory);
	bool TryGetStat(const char* path, uv_stat_t* stat);
};

extern Files g_Files;

#endif //DUOSCRIPT_FILES_H
