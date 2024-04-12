/******************************************************************************
 SymbolUpdatePG.cpp

	Manages a progress display embedded below a widget.

	BASE CLASS = JLatentPG

	Copyright © 2007-24 by John Lindal.

 ******************************************************************************/

#include "SymbolUpdatePG.h"
#include <jx-af/jx/JXContainer.h>
#include <boost/fiber/operations.hpp>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SymbolUpdatePG::SymbolUpdatePG
	(
	JProgressDisplay*	pg,
	const JSize			scaleFactor,
	JXContainer*		widget,
	JXContainer*		container
	)
	:
	JLatentPG(pg, true, scaleFactor),
	itsWidget(widget),
	itsContainer(container)
{
	SetMaxSilentTime(0);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SymbolUpdatePG::~SymbolUpdatePG()
{
}

/******************************************************************************
 ProcessBeginning (virtual protected)

 ******************************************************************************/

void
SymbolUpdatePG::ProcessBeginning
	(
	const ProcessType	processType,
	const JSize			stepCount,
	const JString&		message,
	const bool			allowCancel,
	const bool			modal
	)
{
	itsWidget->AdjustSize(0, -itsContainer->GetFrameHeight());
	itsContainer->Show();

	JLatentPG::ProcessBeginning(processType, stepCount, message,
								allowCancel, modal);
}

/******************************************************************************
 ProcessContinuing (virtual)


 ******************************************************************************/

bool
SymbolUpdatePG::ProcessContinuing()
{
	boost::this_fiber::yield();
	return JLatentPG::ProcessContinuing();
}

/******************************************************************************
 ProcessFinished (virtual)

 ******************************************************************************/

void
SymbolUpdatePG::ProcessFinished()
{
	Hide();
	JLatentPG::ProcessFinished();
}

/******************************************************************************
 Hide

 ******************************************************************************/

void
SymbolUpdatePG::Hide()
{
	itsContainer->Hide();
	itsWidget->AdjustSize(0, itsContainer->GetFrameHeight());
}
