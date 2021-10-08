/******************************************************************************
 Pipe.h

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_Pipe
#define _H_Pipe

#include <jx-af/jcore/JBroadcaster.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/JUtf8ByteBuffer.h>
#include <ace/Svc_Handler.h>
#include <ace/Synch_Traits.h>

class PipeT
{
public:

	enum
		{
		kDefaultBufferSize = 524288		// 512KB
		};

public:

	// Broadcaster messages

	static const JUtf8Byte* kReadReady;

	// Broadcaster message classes

	class ReadReady : public JBroadcaster::Message
		{
		public:

			ReadReady()
				:
				JBroadcaster::Message(kReadReady)
			{ };
		};
};

template <ACE_PEER_STREAM_1>
class Pipe : public ACE_Svc_Handler<ACE_PEER_STREAM_2, ACE_SYNCH>,
			   virtual public JBroadcaster
{
public:

	Pipe();
	Pipe(const ACE_HANDLE fd);

	~Pipe() override;

	void	Read(JString* buffer);
	void	Write(const JString& buffer);

	// ACE_Svc_Handler functions

	int	handle_input(ACE_HANDLE) override;

private:

	JUtf8Byte*		itsRecvBuffer;		// buffer to receive raw bytes
	JSize			itsRecvBufferSize;
	JUtf8ByteBuffer	itsByteBuffer;		// buffer containing unprocessed bytes
	JString			itsRecvData;		// buffer containing unprocessed characters

	bool	itsInHandleInputFlag;	// true => stack passes through handle_input()

private:

	void	PipeX();

	// not allowed

	Pipe(const Pipe&) = delete;
	Pipe& operator=(const Pipe&) = delete;
};

#include "Pipe.tmpl"

#endif
