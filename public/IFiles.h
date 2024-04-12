// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_IFILES_H
#define DUOSCRIPT_IFILES_H

#include <vector>
#include <string>


///	Synchronous API to get files.
///	Similar to IFilesystem, but less verbose.
class IFiles
{
public:
	virtual bool IsFile(const char* fmt, ...) = 0;
	virtual bool IsFolder(const char* fmt, ...) = 0;

	virtual void GetFiles(std::vector<std::string> *files, const char* fmt, ...) = 0;
	virtual void GetDirectories(std::vector<std::string> *directories, const char* fmt, ...) = 0;

public:
	virtual void Read(std::string *contents, const char* fmt, ...) = 0;
	virtual void Write(std::string *newContents, const char* fmt, ...) = 0;
};

#endif //DUOSCRIPT_IFILES_H
