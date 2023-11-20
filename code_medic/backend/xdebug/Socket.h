/******************************************************************************
 Socket.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDSocket
#define _H_XDSocket

#include <jx-af/jcore/JMessageProtocol.h>
#include <ace/SOCK_Stream.h>

namespace xdebug {

class Socket : public JMessageProtocol<ACE_SOCK_STREAM>
{
public:

	Socket();

	~Socket() override;

	void	StartTimer();
	void	StopTimer();

	int	open(void*) override;
	int	handle_timeout(const ACE_Time_Value& time, const void*) override;
	int handle_close(ACE_HANDLE h, ACE_Reactor_Mask m) override;

private:

	long	itsTimerID;
};

};

#endif
