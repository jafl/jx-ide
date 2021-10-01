/******************************************************************************
 SymbolUpdatePG.cpp

	Encapsulates the link back to the parent process which displays the progress.

	Copyright © 2007 by John Lindal.

 ******************************************************************************/

#include "SymbolUpdatePG.h"
#include "ProjectDocument.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SymbolUpdatePG::SymbolUpdatePG
	(
	std::ostream&	link,
	const JSize	scaleFactor
	)
	:
	JProgressDisplay(),
	itsLink(link),
	itsScaleFactor(scaleFactor)
{
}

/******************************************************************************
 Destructor

	Make sure we restore the original signal handler.

 ******************************************************************************/

SymbolUpdatePG::~SymbolUpdatePG()
{
}

/******************************************************************************
 ProcessBeginning (protected)

	Display the message.

 ******************************************************************************/

void
SymbolUpdatePG::ProcessBeginning
	(
	const ProcessType	processType,
	const JSize			stepCount,
	const JString&		message,
	const bool		allowCancel,
	const bool		allowBackground
	)
{
	JProgressDisplay::ProcessBeginning(processType, stepCount, message,
									   allowCancel, allowBackground);

	if (processType == kVariableLengthProcess)
	{
		itsLink << ProjectDocument::kVariableLengthStart;
	}
	else
	{
		assert( processType == kFixedLengthProcess );
		itsLink << ProjectDocument::kFixedLengthStart;
		itsLink << ' ' << stepCount;
	}

	itsLink << ' ' << JString(message) << std::endl;
}

/******************************************************************************
 IncrementProgress

	Update the display to show that progress is being made.
	Returns false if process was cancelled by user.

 ******************************************************************************/

bool
SymbolUpdatePG::IncrementProgress
	(
	const JString&	message,
	const JSize		delta
	)
{
	IncrementStepCount(delta);
	const JSize stepCount = GetCurrentStepCount();

	if (stepCount % itsScaleFactor == 0)
	{
		itsLink << ProjectDocument::kProgressIncrement << ' ' << itsScaleFactor << std::endl;
	}

	return ProcessContinuing();
}

/******************************************************************************
 IncrementProgress

	Update the display to show that progress is being made.
	Returns false if process was cancelled by user.

 ******************************************************************************/

bool
SymbolUpdatePG::IncrementProgress
	(
	const JString& message
	)
{
	return IncrementProgress(message, 1);
}

/******************************************************************************
 IncrementProgress

	The iteration count is incremented by the specified value.

	Update the display to show that progress is being made.
	Returns false if process was cancelled by user.

 ******************************************************************************/

bool
SymbolUpdatePG::IncrementProgress
	(
	const JSize delta
	)
{
	return IncrementProgress(JString::empty, delta);
}

/******************************************************************************
 ProcessFinished

 ******************************************************************************/

void
SymbolUpdatePG::ProcessFinished()
{
	if (GetCurrentProcessType() == kFixedLengthProcess)
	{
		const JSize delta = GetCurrentStepCount() % itsScaleFactor;
		if (delta > 0)
		{
			itsLink << ProjectDocument::kProgressIncrement << ' ' << delta << std::endl;
		}
	}

	JProgressDisplay::ProcessFinished();
}

/******************************************************************************
 CheckForCancel

 ******************************************************************************/

bool
SymbolUpdatePG::CheckForCancel()
{
	return false;
}

/******************************************************************************
 DisplayBusyCursor (virtual)

 ******************************************************************************/

void
SymbolUpdatePG::DisplayBusyCursor()
{
}
