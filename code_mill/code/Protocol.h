/******************************************************************************
 Protocol.h

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_Protocol
#define _H_Protocol

#include <jx-af/jcore/JBroadcaster.h>
#include <jx-af/jcore/JString.h>
#include <ace/Svc_Handler.h>
#include <ace/Synch_Traits.h>

class ProtocolT
{
public:

	enum
		{
		kDefaultBufferSize = 10000
		};

public:

	// Broadcaster messages

	static const JCharacter* kReadReady;

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
class Protocol : public ACE_Svc_Handler<ACE_PEER_STREAM_2, ACE_SYNCH>,
				   virtual public JBroadcaster
{
public:

	Protocol();
	Protocol(const ACE_HANDLE fd);

	virtual ~Protocol();

	void	Read(JString* buffer);
	void	Write(const JCharacter* buffer);

	// ACE_Svc_Handler functions

	virtual int	handle_input(ACE_HANDLE) override;

private:

	JString		itsData;

	JCharacter*	itsBuffer;				// buffer to receive raw bytes
	JSize		itsBufferSize;

	bool		itsInHandleInputFlag;	// true => stack passes through handle_input()

private:

	void	ProtocolX();

	// not allowed

	Protocol(const Protocol&) = delete;
	Protocol& operator=(const Protocol&) = delete;
};

#include <Protocol.tmpl>

#endif
