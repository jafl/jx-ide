/******************************************************************************
 SVNFileInput.cpp

	Input field for entering a file or Subversion URL.

	BASE CLASS = JXFileInput

	Copyright © 2009 by John Lindal.

 ******************************************************************************/

#include "SVNFileInput.h"
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jWebUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SVNFileInput::SVNFileInput
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXFileInput(enclosure, hSizing, vSizing, x,y, w,h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SVNFileInput::~SVNFileInput()
{
}

/******************************************************************************
 AdjustStylesBeforeBroadcast (virtual protected)

	Accept urls

 ******************************************************************************/

void
SVNFileInput::StyledText::AdjustStylesBeforeBroadcast
	(
	const JString&			text,
	JRunArray<JFont>*		styles,
	JStyledText::TextRange*	recalcRange,
	JStyledText::TextRange*	redrawRange,
	const bool			deletion
	)
{
	if (JIsURL(text))
	{
		const JSize totalLength = text.GetCharacterCount();
		JFont f                 = styles->GetFirstItem();
		styles->RemoveAll();
		f.SetColor(JColorManager::GetBlackColor());
		styles->AppendItems(f, totalLength);

		*recalcRange = *redrawRange = JStyledText::TextRange(
			JCharacterRange(1, totalLength),
			JUtf8ByteRange(1, text.GetByteCount()));
	}
	else
	{
		return JXFileInput::StyledText::AdjustStylesBeforeBroadcast(
			text, styles, recalcRange, redrawRange, deletion);
	}
}
