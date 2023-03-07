/******************************************************************************
 XDCloseSocketTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "XDCloseSocketTask.h"
#include "XDSocket.h"

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
