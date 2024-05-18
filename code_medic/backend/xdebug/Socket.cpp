/******************************************************************************
 Socket.cpp

	Socket that connects to Xdebug.

	BASE CLASS = JMessageProtocol

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "xdebug/Socket.h"
#include "xdebug/Link.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

const time_t kClientDeadTime = 5;	// seconds

/******************************************************************************
 Constructor

 ******************************************************************************/

xdebug::Socket::Socket()
	:
	itsTimerID(-1)
{
	SetProtocol("\0", 1, "\1", 1);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

xdebug::Socket::~Socket()
{
	SendDisconnect();
}

/******************************************************************************
 open (virtual)

	This is called when Xdebug connects from a browser.

 ******************************************************************************/

int
xdebug::Socket::open
	(
	void* data
	)
{
	const int result = JMessageProtocol<ACE_SOCK_STREAM>::open(data);
	if (result == 0)
	{
		dynamic_cast<Link&>(*GetLink()).ConnectionEstablished(this);
	}
	return result;
}

/******************************************************************************
 StartTimer

 ******************************************************************************/

void
xdebug::Socket::StartTimer()
{
	StopTimer();

//	itsTimerID = (reactor())->schedule_timer(this, nullptr, ACE_Time_Value(kClientDeadTime));
//	if (itsTimerID == -1)
//		{
//		std::cerr << "Socket::StartTimer() is unable to schedule timeout" << std::endl;
//		}
}

/******************************************************************************
 StopTimer

 ******************************************************************************/

void
xdebug::Socket::StopTimer()
{
	reactor()->cancel_timer(itsTimerID);
}

/******************************************************************************
 handle_timeout (virtual)

	Notify Link that session seems to be closed.

 ******************************************************************************/

int
xdebug::Socket::handle_timeout
	(
	const ACE_Time_Value&	time,
	const void*				data
	)
{
	close();
	return 0;
}

/******************************************************************************
 handle_close (virtual)

	This is called when the connection is closed.

 ******************************************************************************/

int
xdebug::Socket::handle_close
	(
	ACE_HANDLE			h,
	ACE_Reactor_Mask	m
	)
{
	dynamic_cast<Link&>(*GetLink()).ConnectionFinished(this);
	return JMessageProtocol<ACE_SOCK_STREAM>::handle_close(h, m);
}
