/******************************************************************************
 Plot2DCmd.cpp

	BASE CLASS = Command

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "Plot2DCmd.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

Plot2DCmd::Plot2DCmd
	(
	Plot2DDir*	dir,
	JArray<JFloat>*	x,
	JArray<JFloat>*	y
	)
	:
	Command("", false, true),
	itsDirector(dir),
	itsX(x),
	itsY(y)
{
	assert( itsX->GetElementCount() == itsY->GetElementCount() );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

Plot2DCmd::~Plot2DCmd()
{
}

/******************************************************************************
 UpdateRange (virtual)

 ******************************************************************************/

void
Plot2DCmd::UpdateRange
	(
	const JIndex	curveIndex,
	const JInteger	min,
	const JInteger	max
	)
{
	const JSize oldCount = itsX->GetElementCount();
	const JSize newCount = max - min + 1;

	if (newCount < oldCount)
	{
		const JSize delta = oldCount - newCount;
		itsX->RemoveNextElements(oldCount - delta + 1, delta);
		itsY->RemoveNextElements(oldCount - delta + 1, delta);
	}
	else if (oldCount < newCount)
	{
		for (JIndex i=oldCount+1; i<=newCount; i++)
		{
			itsX->AppendElement(0);
			itsY->AppendElement(0);
		}
	}
}
