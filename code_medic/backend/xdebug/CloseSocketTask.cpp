/******************************************************************************
 CloseSocketTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "xdebug/CloseSocketTask.h"
#include "xdebug/Socket.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

xdebug::CloseSocketTask::CloseSocketTask
	(
	Socket* socket
	)
	:
	JXUrgentTask(socket),
	itsSocket(socket)
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

xdebug::CloseSocketTask::~CloseSocketTask()
{
}

/******************************************************************************
 Perform (virtual protected)

 ******************************************************************************/

void
xdebug::CloseSocketTask::Perform()
{
	itsSocket->close();
}
