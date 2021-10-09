/******************************************************************************
 FileDragSource.cpp

	Displays a file icon that the user can drag.

	BASE CLASS = JXImageWidget

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "FileDragSource.h"
#include "SourceDirector.h"
#include <jx-af/jx/JXFileSelection.h>
#include <jx-af/jx/JXDNDManager.h>
#include <jx-af/jx/JXSelectionManager.h>
#include <jx-af/jx/JXWebBrowser.h>
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/JXImageCache.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/jAssert.h>

#include <jx-af/image/jx/jx_plain_file_small.xpm>

/******************************************************************************
 Constructor

 ******************************************************************************/

FileDragSource::FileDragSource
	(
	SourceDirector*	doc,
	JXContainer*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
	JXImageWidget(enclosure, hSizing, vSizing, x,y, w,h),
	itsDoc(doc)
{
	SetImage(GetDisplay()->GetImageCache()->GetImage(jx_plain_file_small), false);

	SetHint(JGetString("Hint::FileDragSource"));
}

/******************************************************************************
 Destructor

 ******************************************************************************/

FileDragSource::~FileDragSource()
{
}

/******************************************************************************
 AdjustCursor (virtual protected)

 ******************************************************************************/

void
FileDragSource::AdjustCursor
	(
	const JPoint&			pt,
	const JXKeyModifiers&	modifiers
	)
{
	const JString* fileName;
	if (!itsDoc->GetFileName(&fileName))
	{
		DisplayCursor(kJXInactiveCursor);
	}
	else
	{
		JXImageWidget::AdjustCursor(pt, modifiers);
	}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
FileDragSource::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	const JString* fileName;
	const bool hasFile = itsDoc->GetFileName(&fileName);
	if (hasFile && clickCount == 2)
	{
		(JXGetWebBrowser())->ShowFileLocation(*fileName);
	}
	else if (hasFile)
	{
		JPtrArray<JString> list(JPtrArrayT::kForgetAll);
		list.Append(const_cast<JString*>(fileName));

		auto* data = jnew JXFileSelection(GetDisplay(), list);
		assert( data != nullptr );

		BeginDND(pt, buttonStates, modifiers, data);
	}
}

/******************************************************************************
 GetDNDAction (virtual protected)

 ******************************************************************************/

Atom
FileDragSource::GetDNDAction
	(
	const JXContainer*		target,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	return GetDNDManager()->GetDNDActionPrivateXAtom();
}

/******************************************************************************
 HandleDNDResponse (virtual protected)

 ******************************************************************************/

void
FileDragSource::HandleDNDResponse
	(
	const JXContainer*	target,
	const bool		dropAccepted,
	const Atom			action
	)
{
	DisplayCursor(GetDNDManager()->GetDNDFileCursor(dropAccepted, action));
}
