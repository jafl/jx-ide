/******************************************************************************
 ProcessText.cpp

	BASE CLASS = public JXStaticText

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#include "ProcessText.h"
#include "ChooseProcessDialog.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

ProcessText::ProcessText
	(
	ChooseProcessDialog*	dir,
	JXScrollbarSet*			scrollbarSet,
	JXContainer*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
	JXStaticText(JString::empty, true, true, true, scrollbarSet,
				 enclosure, hSizing, vSizing, x,y, w, h),
	itsDir(dir)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

ProcessText::~ProcessText()
{
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
ProcessText::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JXStaticText::HandleMouseDown(pt, button, clickCount, buttonStates, modifiers);
	JString text;
	if (clickCount == 2 && GetSelection(&text))
	{
		JInteger value;
		if (text.ConvertToInteger(&value))
		{
			itsDir->SetProcessID(value);
			itsDir->EndDialog(true);
		}
	}
}
