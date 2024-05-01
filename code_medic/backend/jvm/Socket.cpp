/******************************************************************************
 Socket.cpp

	Socket that connects to JVM.

	BASE CLASS = JNetworkProtocolBase, virtual JBroadcaster

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "jvm/Socket.h"
#include "jvm/Link.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

static const JString kHandshake("JDWP-Handshake");
const JSize kBufferSize      = 65536;	// 64KB
const time_t kClientDeadTime = 5;		// seconds

// Broadcaster messages

const JUtf8Byte* jvm::Socket::kMessageReady = "MessageReady::JVMSocket";

/******************************************************************************
 Constructor

 ******************************************************************************/

jvm::Socket::Socket()
	:
	JNetworkProtocolBase<ACE_SOCK_STREAM>(false),
	itsHandshakeFinishedFlag(false),
	itsTimerID(-1),
	itsInHandleInputFlag(false)
{
	itsBuffer   = jnew unsigned char [ kBufferSize ];
	itsRecvData = jnew JArray<unsigned char>;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

jvm::Socket::~Socket()
{
	jdelete [] itsBuffer;
	jdelete itsRecvData;
}

/******************************************************************************
 open (virtual)

	This is called when the jvm connects.

 ******************************************************************************/

int
jvm::Socket::open
	(
	void* data
	)
{
	const int result = JNetworkProtocolBase<ACE_SOCK_STREAM>::open(data);
	if (result == 0)
	{
		JNetworkProtocolBase<ACE_SOCK_STREAM>::Send(kHandshake);
		dynamic_cast<Link*>(GetLink())->ConnectionEstablished(this);
		StartTimer();
	}
	return result;
}

/******************************************************************************
 handle_close (virtual)

	This is called when the connection is closed.

 ******************************************************************************/

int
jvm::Socket::handle_close
	(
	ACE_HANDLE			h,
	ACE_Reactor_Mask	m
	)
{
	dynamic_cast<Link*>(GetLink())->ConnectionFinished(this);
	return JNetworkProtocolBase<ACE_SOCK_STREAM>::handle_close(h, m);
}

/******************************************************************************
 handle_input (virtual)

	This is called when we receive data.  It is also safe for anybody else
	to call it.

 ******************************************************************************/

int
jvm::Socket::handle_input
	(
	ACE_HANDLE
	)
{
	// protect against recursive calls

	if (itsInHandleInputFlag)
	{
		return 0;
	}
	itsInHandleInputFlag = true;

	// read data from the socket

	const ssize_t count = (ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_SYNCH>::peer()).recv(itsBuffer, kBufferSize);
	if (count > 0)
	{
		StopTimer();

		for (JUnsignedOffset i=0; i<count; i++)
		{
			itsRecvData->AppendItem(itsBuffer[i]);
		}

		if (!itsHandshakeFinishedFlag &&
			memcmp(itsRecvData->GetCArray(), kHandshake.GetBytes(), kHandshake.GetByteCount()) == 0)
		{
			itsHandshakeFinishedFlag = true;
			itsRecvData->RemoveNextItems(1, kHandshake.GetByteCount());

			dynamic_cast<Link*>(GetLink())->InitDebugger();
		}

		while (itsHandshakeFinishedFlag && itsRecvData->GetItemCount() >= 11)
		{
			const unsigned char* msg = itsRecvData->GetCArray();
			const JSize msgLength    = Unpack4(msg);
			if (itsRecvData->GetItemCount() >= msgLength)
			{
				const JIndex id        = Unpack4(msg + 4);
				const bool isReply = *(msg + 8);
				const JIndex cmdSet    = *(msg + 9);		// non-reply
				const JIndex cmd       = *(msg + 10);		// non-reply
				const JIndex errorCode = Unpack2(msg + 9);	// reply

				Broadcast(MessageReady(id, isReply, cmdSet, cmd, errorCode,
									   msg + 11, msgLength - 11));

				itsRecvData->RemoveNextItems(1, msgLength);
			}
			else
			{
				break;
			}
		}
	}

	itsInHandleInputFlag = false;
	return 0;
}

/******************************************************************************
 Send

 ******************************************************************************/

void
jvm::Socket::Send
	(
	const JIndex			id,
	const JIndex			cmdSet,
	const JIndex			cmd,
	const unsigned char*	data,
	const JSize				count
	)
{
	std::ostringstream log;
	log << "send: " << id << ' ' << cmdSet << ' ' << cmd;
	Link::Log(log);

	unsigned char header[11];
	Pack4(count+11, header);	// total length
	Pack4(id, header+4);		// id
	header[8]  = 0;				// flags
	header[9]  = cmdSet;		// command set
	header[10] = cmd;			// command

	if (count == 0)
	{
		JNetworkProtocolBase<ACE_SOCK_STREAM>::Send((const JUtf8Byte*) header, 11);
	}
	else
	{
		iovec buffer[2];
		buffer[0].iov_base = header;
		buffer[0].iov_len  = 11;
		buffer[1].iov_base = const_cast<unsigned char*>(data);
		buffer[1].iov_len  = count;
		JNetworkProtocolBase<ACE_SOCK_STREAM>::Send(buffer, 2);
	}

	StartTimer();
}

/******************************************************************************
 StartTimer (private)

 ******************************************************************************/

void
jvm::Socket::StartTimer()
{
	StopTimer();

	itsTimerID = (reactor())->schedule_timer(this, nullptr, ACE_Time_Value(kClientDeadTime));
	if (itsTimerID == -1)
	{
		std::cerr << "JVMSocket::StartTimer() is unable to schedule timeout" << std::endl;
	}
}

/******************************************************************************
 StopTimer (private)

 ******************************************************************************/

void
jvm::Socket::StopTimer()
{
	(reactor())->cancel_timer(itsTimerID);
}

/******************************************************************************
 handle_timeout (virtual)

	Notify Link that session seems to be closed.

 ******************************************************************************/

int
jvm::Socket::handle_timeout
	(
	const ACE_Time_Value&	time,
	const void*				data
	)
{
	close();
	return 0;
}

/******************************************************************************
 Pack (static)

 ******************************************************************************/

void
jvm::Socket::Pack
	(
	const JSize		size,
	const JUInt64	value,
	unsigned char*	data
	)
{
	if (size == 2)
	{
		Pack2(value, data);
	}
	else if (size == 4)
	{
		Pack4(value, data);
	}
	else if (size == 8)
	{
		Pack8(value, data);
	}
}

/******************************************************************************
 Unpack (static)

 ******************************************************************************/

JUInt64
jvm::Socket::Unpack
	(
	const JSize				size,
	const unsigned char*	data
	)
{
	if (size == 2)
	{
		return Unpack2(data);
	}
	else if (size == 4)
	{
		return Unpack4(data);
	}
	else if (size == 8)
	{
		return Unpack8(data);
	}
	else
	{
		return 0;
	}
}

/******************************************************************************
 Pack2 (static)

 ******************************************************************************/

void
jvm::Socket::Pack2
	(
	const JUInt32	value,
	unsigned char*	data
	)
{
	for (int i=0; i<2; i++)
	{
		data[i] = (value >> (8 * (1-i))) & 0xFF;
	}
}

/******************************************************************************
 Unpack2 (static)

 ******************************************************************************/

JUInt32
jvm::Socket::Unpack2
	(
	const unsigned char* data
	)
{
	JUInt32 result = 0;
	for (int i=0; i<2; i++)
	{
		result |= data[i] << (8 * (1-i));
	}

	return result;
}

/******************************************************************************
 Pack4 (static)

 ******************************************************************************/

void
jvm::Socket::Pack4
	(
	const JUInt32	value,
	unsigned char*	data
	)
{
	for (int i=0; i<4; i++)
	{
		data[i] = (value >> (8 * (3-i))) & 0xFF;
	}
}

/******************************************************************************
 Unpack4 (static)

 ******************************************************************************/

JUInt32
jvm::Socket::Unpack4
	(
	const unsigned char* data
	)
{
	JUInt32 result = 0;
	for (int i=0; i<4; i++)
	{
		result |= data[i] << (8 * (3-i));
	}

	return result;
}

/******************************************************************************
 Pack8 (static)

 ******************************************************************************/

void
jvm::Socket::Pack8
	(
	const JUInt64	value,
	unsigned char*	data
	)
{
	for (int i=0; i<8; i++)
	{
		data[i] = (value >> (8 * (7-i))) & 0xFF;
	}
}

/******************************************************************************
 Unpack8 (static)

 ******************************************************************************/

JUInt64
jvm::Socket::Unpack8
	(
	const unsigned char* data
	)
{
	JUInt64 result = 0;
	for (int i=0; i<8; i++)
	{
		result |= data[i] << (8 * (7-i));
	}

	return result;
}

/******************************************************************************
 PackString (static)

 ******************************************************************************/

void
jvm::Socket::PackString
	(
	const JString&	s,
	unsigned char*	data
	)
{
	const JSize length = s.GetByteCount();

	Pack4(length, data);
	memcpy(data+4, s.GetBytes(), length);
}

/******************************************************************************
 UnpackString (static)

 ******************************************************************************/

JString
jvm::Socket::UnpackString
	(
	const unsigned char*	data,
	JSize*					dataLength
	)
{
	const JSize length = Unpack4(data);
	*dataLength        = 4 + length;
	return JString((const JUtf8Byte*) (data+4), length);
}
