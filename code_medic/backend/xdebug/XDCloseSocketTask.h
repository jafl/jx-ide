/******************************************************************************
 XDCloseSocketTask.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDCloseSocketTask
#define _H_XDCloseSocketTask

#include <jx-af/jx/JXUrgentTask.h>

namespace xdebug {

class Socket;

class CloseSocketTask : public JXUrgentTask
{
public:

	CloseSocketTask(Socket* socket);

	~CloseSocketTask() override;

	void	Perform() override;

private:

	Socket*	itsSocket;
};

};

#endif
