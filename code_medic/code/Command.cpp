/******************************************************************************
 Command.cpp

	Command encapsulates all the information necessary to submit a gdb
	command to a Link and to uniquely identify resulting broadcasts.  It
	is designed to greatly reduce the amount of state information that
	clients must maintain about the commands they have submitted and
	generally loosen the coupling between objects in several ways.

	First, each command has a unique transaction ID which is assigned when
	the command is submitted to the queue.  This allows the same command
	object to be submitted many times without compromising the ability of
	the client to distinguish separate transactions.

	Second, if the client maintains a reference to the command while it is
	in the queue, the command provides information about the current status
	of the command.  In particular, commands automatically know how to
	submit and remove themselves from the command queue.

	Third, the command itself is automatically notified of the results via
	HandleSuccess() and HandleFailure(), so in many cases it can act on the
	results itself.

	Each command has an intrinsic state which reflects what is happening to it
	at any given moment.  kUnassigned means the command is not currently
	executing or waiting to be executed.

	The other two states indicate various stages of the command's
	processing.  In these states the command has been assigned a
	transaction ID.  kPending means the command is waiting in either the
	background and foreground queue.  kExecuting means the command is
	currently executing within the debugger.

	BASE CLASS = none

	Copyright (C) 1997-2001 by John Lindal.

 *****************************************************************************/

#include "Command.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

	The second version allows commands to be built on the fly in the call
	to the base class constructor.

 *****************************************************************************/

Command::Command
	(
	const JUtf8Byte*	cmd,
	const bool			oneShot,
	const bool			background
	)
	:
	itsCommandString(cmd),
	itsResultList(nullptr),
	itsID(0),
	itsState(kUnassigned),
	itsDeleteFlag(oneShot),
	itsBackgroundFlag(background),
	itsIgnoreResultFlag(false)
{
}

Command::Command
	(
	const JString&	cmd,
	const bool	oneShot,
	const bool	background
	)
	:
	itsCommandString(cmd),
	itsResultList(nullptr),
	itsID(0),
	itsState(kUnassigned),
	itsDeleteFlag(oneShot),
	itsBackgroundFlag(background),
	itsIgnoreResultFlag(false)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

Command::~Command()
{
	jdelete itsResultList;

	if (itsState != kUnassigned)
	{
		GetLink()->Cancel(this);
	}
}

/******************************************************************************
 SetTransactionID

 *****************************************************************************/

void
Command::SetTransactionID
	(
	const JIndex id
	)
{
	assert( id != 0 );
	itsID    = id;
	itsState = kPending;
}

/******************************************************************************
 Send

 *****************************************************************************/

bool
Command::Send()
{
	return GetLink()->Send(this);
}

/******************************************************************************
 Starting (virtual)

 *****************************************************************************/

void
Command::Starting()
{
	itsState = kExecuting;
	itsData.Clear();
}

/******************************************************************************
 Finished

 *****************************************************************************/

void
Command::Finished
	(
	const bool success
	)
{
	itsState = kUnassigned;		// before Handle*() to allow re-send

	if (success)
	{
		HandleSuccess(itsData);
	}
	else
	{
		HandleFailure();
	}

	if (itsDeleteFlag)
	{
		jdelete this;
	}
	else
	{
		itsData.Clear();	// don't waste space
	}
}

/******************************************************************************
 HandleFailure (virtual protected)

	Default empty implementation provided because most commands won't need
	to do anything if they fail.

 *****************************************************************************/

void
Command::HandleFailure()
{
}

/******************************************************************************
 Result

	Mainly for gdb, so it isn't passed to HandleSuccess().

 *****************************************************************************/

static const JString theEmptyResult;

const JPtrArray<JString>&
Command::GetResults()
	const
{
	if (itsResultList == nullptr)
	{
		const_cast<Command*>(this)->itsResultList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
		assert( itsResultList != nullptr );
	}

	return *itsResultList;
}

const JString&
Command::GetLastResult()
	const
{
	return (itsResultList != nullptr ? *(itsResultList->GetLastElement()) : theEmptyResult);
}

void
Command::SaveResult
	(
	const JString& data
	)
{
	if (itsResultList == nullptr)
	{
		itsResultList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
		assert( itsResultList != nullptr );
	}

	itsResultList->Append(data);
}
