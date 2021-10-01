/******************************************************************************
 StylerTableInput.cpp

	Class to edit a string "in place" in a JXEditTable.

	BASE CLASS = JXInputField

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "StylerTableInput.h"
#include "StylerTable.h"
#include "sharedUtil.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

StylerTableInput::StylerTableInput
	(
	const TextFileType	fileType,
	StylerTable*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
	JXInputField(enclosure, hSizing, vSizing, x,y, w,h)
{
	itsStylerTable = enclosure;
	itsFileType    = fileType;

	PrefsManager* prefsMgr = GetPrefsManager();
	SetCaretColor(prefsMgr->GetColor(PrefsManager::kCaretColorIndex));
	SetSelectionColor(prefsMgr->GetColor(PrefsManager::kSelColorIndex));
	SetSelectionOutlineColor(prefsMgr->GetColor(PrefsManager::kSelLineColorIndex));

	GetText()->SetCharacterInWordFunction([this] (const JUtf8Character& c)
	{
		return ::IsCharacterInWord(c) || LangIsCharacterInWord(this->itsFileType, c);
	});
}

/******************************************************************************
 Destructor

 ******************************************************************************/

StylerTableInput::~StylerTableInput()
{
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
StylerTableInput::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if ((button == kJXLeftButton && modifiers.meta()) ||
		button == kJXRightButton)
	{
		JPoint cell;
		const bool ok = itsStylerTable->GetEditedCell(&cell);
		assert( ok );
		itsStylerTable->DisplayFontMenu(cell, this, pt, buttonStates, modifiers);
	}
	else
	{
		JXInputField::HandleMouseDown(pt, button, clickCount,
									  buttonStates, modifiers);
	}
}
