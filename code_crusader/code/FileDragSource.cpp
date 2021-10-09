/******************************************************************************
 FileDragSource.cpp

	Displays a file icon that the user can drag.

	BASE CLASS = JXImageWidget

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#include "FileDragSource.h"
#include "TextDocument.h"
#include "DSSFinishSaveTask.h"
#include <jx-af/jx/JXFileSelection.h>
#include <jx-af/jx/JXDSSSelection.h>
#include <jx-af/jx/JXDirectSaveSource.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXWebBrowser.h>
#include <jx-af/jx/JXImageCache.h>
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

#include <jx-af/image/jx/jx_plain_file_small.xpm>

/******************************************************************************
 Constructor

 ******************************************************************************/

FileDragSource::FileDragSource
	(
	TextDocument*		doc,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXImageWidget(enclosure, hSizing, vSizing, x,y, w,h),
	itsDoc(doc)
{
	SetImage(GetDisplay()->GetImageCache()->GetImage(jx_plain_file_small), false);

	ProvideDirectSave(nullptr);
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
	if ((itsNameInput == nullptr && !itsDoc->ExistsOnDisk()) ||
		(itsNameInput != nullptr &&
		 itsNameInput->GetText()->GetText().EndsWith(ACE_DIRECTORY_SEPARATOR_STR)))
	{
		DisplayCursor(kJXInactiveCursor);
		SetHint(JGetString("kFileSelected::JXFSDirMenu"));
	}
	else
	{
		if (itsNameInput != nullptr && itsNameInput->GetText()->IsEmpty())
		{
			SetHint(JGetString("HowToXDS2HintID::FileDragSource"));
		}
		else if (itsNameInput != nullptr)
		{
			SetHint(JGetString("XDSHint::FileDragSource"));
		}
		else
		{
			SetHint(JGetString("XDNDHint::FileDragSource"));
		}

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
	bool onDisk;
	JString fileName = itsDoc->GetFullName(&onDisk);
	if (clickCount == 2 && onDisk)
	{
		JXGetWebBrowser()->ShowFileLocation(fileName);
	}
	else if (itsNameInput == nullptr && onDisk)
	{
		JPtrArray<JString> list(JPtrArrayT::kForgetAll);
		list.Append(&fileName);

		auto* data = jnew JXFileSelection(GetDisplay(), list);
		assert( data != nullptr );

		BeginDND(pt, buttonStates, modifiers, data);
	}
	else if (itsNameInput != nullptr &&
			 !itsNameInput->GetText()->GetText().EndsWith(ACE_DIRECTORY_SEPARATOR_STR))
	{
		auto* task = jnew DSSFinishSaveTask(itsDoc);
		assert( task != nullptr );

		auto* data = jnew JXDSSSelection(GetWindow(), task);
		assert( data != nullptr );

		BeginDND(pt, buttonStates, modifiers, data);
	}
}

/******************************************************************************
 DNDInit (virtual protected)

 ******************************************************************************/

void
FileDragSource::DNDInit
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsNameInput != nullptr && !itsNameInput->GetText()->IsEmpty())
	{
		JString fullName = itsNameInput->GetText()->GetText();
		JStripTrailingDirSeparator(&fullName);		// paranoia -- checked earlier

		JString path, name;
		JSplitPathAndName(fullName, &path, &name);

		JXDirectSaveSource::Init(GetWindow(), name);
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
	if (itsNameInput != nullptr)
	{
		return GetDNDManager()->GetDNDActionDirectSaveXAtom();
	}
	else
	{
		return GetDNDManager()->GetDNDActionCopyXAtom();
	}
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
