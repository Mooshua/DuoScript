// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#ifndef DUOSCRIPT_LOGIC_H
#define DUOSCRIPT_LOGIC_H

#include <uv.h>

#include <ILoop.h>
#include "Files/FileSystemController.h"
#include "Files/ZipController.h"
#include "Generic/LogController.h"
#include "Generic/PerformanceController.h"
#include "Memory/PointerController.h"
#include "Threading/FiberController.h"


class DuoLogic
{
public:
	ILogger* _logger;
	ILoop* _loop;
	IFiles* _files;

	DuoLogic(ILogger* logger, ILoop* loop, IFiles* files);

public:
	void OnScriptReady(IScriptControllerManager* manager);

public:
	FileController* _fileController;
	ZipController* _zipController;
	LogController* _logController;
	PerformanceController* _performanceController;
	PointerController* _pointerController;
	FiberController* _fiberController;
};


#endif //DUOSCRIPT_LOGIC_H
