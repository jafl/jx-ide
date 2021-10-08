/******************************************************************************
 XDCloseSocketTask.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDCloseSocketTask
#define _H_XDCloseSocketTask

#include <jx-af/jx/JXUrgentTask.h>

class XDSocket;

class XDCloseSocketTask : public JXUrgentTask
{
public:

	XDCloseSocketTask(XDSocket* socket);

	~XDCloseSocketTask();

	void	Perform() override;

private:

	XDSocket*	itsSocket;
};

#endif
