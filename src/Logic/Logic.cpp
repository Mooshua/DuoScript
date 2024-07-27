// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#include "Logic.h"

void DuoLogic::OnScriptReady(IScriptControllerManager *manager)
{
	manager->Register(_fileController);
	manager->Register(_fiberController);
	manager->Register(_logController);
	manager->Register(_performanceController);
	manager->Register(_pointerController);
	manager->Register(_zipController);
}

DuoLogic::DuoLogic(ILogger *logger, ILoop *loop, IFiles* files)
{
	this->_logger = logger;
	this->_loop = loop;
	this->_files = files;

	this->_fileController = new FileController(_logger, _loop, _files);
	this->_fiberController = new FiberController(_logger, _loop);
	this->_logController = new LogController();
	this->_performanceController = new PerformanceController();
	this->_pointerController = new PointerController();
	this->_zipController = new ZipController(_loop);

}
